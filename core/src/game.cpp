#include "gamelib/game.h"
#include <filesystem>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <GLFW/glfw3.h>
#include <thread>

Game::Game() : _window(nullptr), _roomFactory(nullptr) {
	_frameTime = 1.0 / 60.0;
	std::cout << "Welcome to gamelib!" << std::endl;
}

void Game::init(const std::string &homeDir) {
	try {
		_homeDir = homeDir;
		_config = std::make_unique<Config>(homeDir + "/config.yaml");
		initGL();
		if (_config->mouseOn()) {
			glfwSetMouseButtonCallback(_window, mouse_button_callback);
			glfwSetCursorPosCallback(_window, cursor_pos_callback);
		}
	} catch (const std::exception &e) {
		std::cerr << "Failed to load config: " << e.what() << std::endl;
	}
}

void Game::closeGL() {
	if (_window != nullptr) {
		glfwDestroyWindow(_window);
		_window = nullptr;
	}
	glfwTerminate();
}

void Game::initGL() {
	if(!glfwInit())
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		exit(1);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	auto windowSize = _config->getWindowSize();
	_window = glfwCreateWindow( windowSize.x, windowSize.y, _config->getTitle().c_str(), NULL, NULL);
	if( _window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(_window);
	glfwSetFramebufferSizeCallback(_window, Game::WindowResizeCallback);
	glfwSetKeyCallback(_window, key_callback);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);

	std::cout << "OpenGL Version: " << version << std::endl;
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "Vendor: " << vendor << std::endl;
	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		exit(1);
	}
	Game::WindowResizeCallback(_window, windowSize.x, windowSize.y);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::WindowResizeCallback(GLFWwindow *win, int width, int height) {
	if (height == 0) {
		height = 1;
	}
	Game::instance()._screenHeight = height;
	auto& game = Game::instance();
	auto* config = game.getConfig();
	float winAspectRatio = static_cast<float>(width) / height;
	auto deviceSize = config->getDeviceSize();
	auto dar = config->getDeviceAspectRatio();
	int vx, vy, vw, vh;
	if (winAspectRatio > dar) {
		// vertical bands
		vw = (int) (height * dar);
		vh = (int) (vw / dar);
		vx = (int) ((width - vw) / 2);
		vy = (int) ((height - vh) / 2);
	} else {
		// horizontal bands
		vw = width;
		vh = (int) (width / dar);
		vx = 0;
		vy = (int) ((height - vh) / 2);
	}

	game._windowViewport = glm::vec4(vx, vy, vw, vh);
}

void Game::run() {

	bool shutdown = false;
	while (!shutdown) {
		if (!_roomFactory) throw std::runtime_error("No room factory set!");
		_room = _roomFactory->createRoom();
		_roomFactory->init(_room);
		_room->initialize();
		bool endRoom = false;
		// main loop
		do {
			double currentTime = glfwGetTime();
			/// note: if I run the update only every frame time CPU goes to 100%. If I run it on
			/// every iter, it doesn't. Tried move the glfwSwapBuffers call (and successive) out of the loop
			/// and that seems to work.
			// Wait until it's time for the next frame
			while (currentTime - _timeLastUpdate < _frameTime) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Yield CPU
				currentTime = glfwGetTime();
			}
			double dt = _frameTime; //currentTime - _timeLastUpdate;
			_timeLastUpdate = currentTime;

			_room->update(dt);

			_room->draw();

			glfwSwapBuffers(_window);
			glfwPollEvents();

			shutdown = glfwWindowShouldClose(_window) != 0;
			endRoom = _room->isComplete();

		} while (!endRoom && !shutdown);
		_room->cleanup();
	}

}

IShader* Game::getShader(const std::string &id) {
	return _shaderRegistry.getShader(id);
}

void Game::setRoomFactory(std::shared_ptr<IRoomFactory> factory) {
	_roomFactory = factory;
}

Room *Game::getCurrentRoom() {
	return _room.get();
}

void Game::setCurrentRoom(std::shared_ptr<Room> room) {
	_room = room;
}


void Game::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	for (auto& s : Game::instance()._keyboardListeners) {
		auto retval = s->keyCallback(window, key, scancode, action, mods);
		if (retval == 1) {
			break;
		}
	}
}


void Game::registerToKeyboardEvent(KeyListener* listener) {
	_keyboardListeners.insert(listener);
}

void Game::unregisterToKeyboardEvent(KeyListener* listener) {
	_keyboardListeners.erase(listener);
}

void Game::mouse_button_callback(GLFWwindow* win, int button, int action, int mods) {
	for (auto &listener : Game::instance()._mouseListeners) {
		if (listener->mouseButtonCallback(win, button, action, mods) == 0) break;
	}
}

void Game::cursor_pos_callback(GLFWwindow * win, double xpos, double ypos) {
	for (auto &listener : Game::instance()._mouseListeners) {
		listener->cursorPosCallback(win, xpos, ypos);
	}
}

void Game::registerToMouseEvent(MouseListener* listener) {
	_mouseListeners.insert(listener);
}

void Game::unregisterToMouseEvent(MouseListener * listener) {
	_mouseListeners.erase(listener);

}

glm::vec2 Game::getDeviceCoordinates(glm::vec2 s) {
	auto deviceSize = _config->getDeviceSize();
	float devx = (s.x - _windowViewport.x) * deviceSize.x / _windowViewport[2];
	float devy = (_screenHeight - s.y - _windowViewport.y) * deviceSize.y / _windowViewport[3];
	return {devx, devy};
}
