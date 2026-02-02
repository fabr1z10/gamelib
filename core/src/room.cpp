#include "gamelib/room.h"
#include <GL/glew.h>
#include <stdexcept>
#include "gamelib/game.h"
#include "gamelib/node.h"
#include "gamelib/batch_shader.h"
#include "gamelib/shaders/basic_shaders.h"


Room::Room() : _clearColor(0.0f, 0.0f, 0.0f), _paused(false) {
	_rootNode = std::make_shared<Node>();
	_blitShader = std::make_shared<IShader>(
			gamelib::shaders::blit_vertex, gamelib::shaders::blit_fragment, "2f2f");
	auto deviceSize = Game::instance().getConfig()->getDeviceSize();
	float quadVertices[] = {
			// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f
	};
	// screen quad VAO
	glGenVertexArrays(1, &_quadVAO);
	glGenBuffers(1, &_quadVBO);
	glBindVertexArray(_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// create frame buffer
	glGenFramebuffers(1, &_fb);
	glBindFramebuffer(GL_FRAMEBUFFER, _fb);
	glGenTextures(1, &_color);
	glBindTexture(GL_TEXTURE_2D, _color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, deviceSize.x, deviceSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _color, 0);

	// We also want to make sure OpenGL is able to do depth testing (and optionally stencil testing) so we have to make
	// sure to add a depth (and stencil) attachment to the framebuffer. Since we'll only be sampling the color buffer
	// and not the other buffers we can create a renderbuffer object for this purpose.

	// Creating a renderbuffer object isn't too hard. The only thing we have to remember is that we're creating it as a
	// depth and stencil attachment renderbuffer object. We set its internal format to GL_DEPTH24_STENCIL8 which
	// is enough precision for our purposes:

	glGenTextures(1, &_depth);
	glBindTexture(GL_TEXTURE_2D, _depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, deviceSize.x, deviceSize.y, 0,  GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_scheduler = std::make_shared<Scheduler>();

}


void Room::setClearColor(glm::ivec3 color) {
	_clearColor = glm::vec3(color) / 255.0f;
}

void Room::initialize() {
	for (auto& b : _batches) {
		b.second->configure();
	}
}
void Room::draw() {
	// render on the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, _fb);
	glEnable(GL_DEPTH_TEST);
	glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1st draw dynamic batches
	for (const auto& b : _batches) {
		if (b.second->isDynamic()) {
			b.second->draw();
		}
	}

	// now draw all static models
	for (const auto& shader : _shadersForStaticRendering) {
		shader->use();
		_rootNode->draw(shader);
	}


	auto vp = Game::instance().getWindowViewport();
	//std::vector<float> ccc(256*240);
	//glReadPixels(0,0,256,240, GL_DEPTH_COMPONENT, GL_FLOAT, &ccc[0]);
	// frame buffer rendering start
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
	glViewport(vp.x, vp.y, vp.z, vp.w);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_blitShader->use();
	glBindVertexArray(_quadVAO);
	_blitShader->setInt("screenTexture", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _color);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);

}

void Room::addShader(IShader* shader) {
	_shadersForStaticRendering.insert(shader);
}
void Room::addBatch(const std::string &key, std::shared_ptr<IBatch> batch) {
	_batches[key] = batch;
	if (!batch->isDynamic()) {
		// for static batches, we need to store the shader. Static models are rendered
		// by looping through the scene graph i.e. we need one loop for each shader
		_shadersForStaticRendering.insert(batch->getShader());
	}
}

void Room::addCamera(const std::string &key, std::shared_ptr<Camera> cam) {
	_cameras[key] = cam;
}

IBatch *Room::getBatch(const std::string & key) {
	try {
		return _batches.at(key).get();
	} catch (const std::out_of_range&) {
		std::cerr << ("Batch with key '" + key + "' not found");
		return nullptr;

	}
}

Camera *Room::getCamera(const std::string &key) {
	try {
		return _cameras.at(key).get();
	} catch (const std::out_of_range&) {
		throw std::runtime_error("Cam with key '" + key + "' not found");
	}
}

void Room::update(double dt) {
	if (!_paused) {
		_scheduler->update(dt);
		_rootNode->update(dt);
	}
}

void Room::addCollisionEngine(std::shared_ptr<ICollisionEngine> engine) {
	_collisionEngine = engine;
}

void Room::cleanup() {
	_rootNode = nullptr;

}

void Room::play(std::shared_ptr<Script> script) {
	_scheduler->play(script);
}

void Room::pause(bool value) {
	_paused = value;
	getRootNode()->setActive(!value);
}