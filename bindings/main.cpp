#include <pybind11/pybind11.h>
#include <gamelib/game.h>
#include <gamelib/shader.h>

#include "pyroomfactory.h"
#include "gamelib/tex.h"
#include "gamelib/spritesheet.h"
#include "gamelib/model.h"
#include "gamelib/node.h"

namespace py = pybind11;

PYBIND11_MODULE(gamelib, mainModule) {
	py::class_<Game>(mainModule, "Game", py::dynamic_attr())
			.def_static("instance", &Game::instance, py::return_value_policy::reference)
			.def("getShader", &Game::getShader, py::arg("id"), py::return_value_policy::reference)
			.def("init", &Game::init, py::arg("homeDir"))
			.def("setRoomFactory", &Game::setRoomFactory, py::arg("factory"), py::keep_alive<1, 2>())
			.def("run", &Game::run);

	py::class_<IBatch, std::shared_ptr<IBatch>>(mainModule, "IBatch")
		.def("getModel", &IBatch::getModel)
		.def("addSpriteSheet", &IBatch::addSpriteSheet, py::arg("spritesheet"));

	py::class_<IModel, std::shared_ptr<IModel>>(mainModule, "IModel");

	py::class_<IShader>(mainModule, "Shader", py::dynamic_attr())
			.def("getBatch", &IShader::createBatch);

	py::class_<IRoomFactory, PyRoomFactory, std::shared_ptr<IRoomFactory>>(mainModule, "RoomFactory")
			.def(py::init<>())
			.def("createRoom", &IRoomFactory::createRoom);

	py::class_<Room, std::shared_ptr<Room>>(mainModule, "Room")
			.def(py::init<>())
			.def_property("root", &Room::getRootNode, nullptr)
			.def("getBatch", &Room::getBatch, py::return_value_policy::reference)
			.def("addBatch", &Room::addBatch, py::arg("key"), py::arg("batch"))
			.def("addCamera", &Room::addCamera, py::arg("key"), py::arg("camera"))
			.def("setClearColor", [](Room &self, int r, int g, int b) {
				self.setClearColor(glm::ivec3{r, g, b});
			}, py::arg("r"), py::arg("g"), py::arg("b"));

	py::class_<Camera, std::shared_ptr<Camera>>(mainModule, "Camera")
			.def("setPosition", [](Camera &self, py::tuple eye, py::tuple direction = py::make_tuple(0.f, 0.f, -1.f),
								   py::tuple up = py::make_tuple(0.f, 1.f, 0.f)) {
				self.setPosition(
						glm::vec3(eye[0].cast<float>(), eye[1].cast<float>(), eye[2].cast<float>()),
						glm::vec3(direction[0].cast<float>(), direction[1].cast<float>(), direction[2].cast<float>()),
						glm::vec3(up[0].cast<float>(), up[1].cast<float>(), up[2].cast<float>())
				);
			})
			.def("move", [](Camera &self, py::tuple delta) {
				self.move(glm::vec3(delta[0].cast<float>(), delta[1].cast<float>(), delta[2].cast<float>()));
			})
			.def("setBounds", [](Camera &self, float xMin, float xMax, float yMin, float yMax, float zMin, float Zmax) {
				self.setBounds(
						glm::vec3(xMin, yMin, zMin),
						glm::vec3(xMax, yMax, Zmax)
				);
			}, py::arg("xMin"), py::arg("xMax"), py::arg("yMin"), py::arg("yMax"), py::arg("zMin"), py::arg("zMax"));


	py::class_<OrthoCamera, Camera, std::shared_ptr<OrthoCamera>>(mainModule, "OrthoCamera")
		.def(py::init([](float width, float height, py::tuple vp) {
			return std::make_shared<OrthoCamera>(width, height, glm::vec4(vp[0].cast<float>(),vp[1].cast<float>(), vp[2].cast<float>(),vp[3].cast<float>()));
		}), py::arg("width"), py::arg("height"), py::arg("viewport") = py::make_tuple(0.f, 0.f, 0.f, 0.f))
		.def("size", [](OrthoCamera &self) {
			return py::make_tuple(self.getSize().x, self.getSize().y);
		});

	py::class_<Tex, std::shared_ptr<Tex>>(mainModule, "Tex")
		.def_static("getTexture", &Tex::getTexture, py::arg("filename"))
		.def("size", [](Tex &self) {
			return py::make_tuple(self.getSize().x, self.getSize().y);
		});

	py::class_<SpriteSheet, std::shared_ptr<SpriteSheet>>(mainModule, "SpriteSheet")
		.def_static("getSheet", &SpriteSheet::getSpriteSheet, py::arg("filename"));


	py::class_<QuadModel, IModel, std::shared_ptr<QuadModel>>(mainModule, "QuadModel");
		//.def(py::init<IBatch*, int, int, int, int>(),
		     //   py::arg("batch"), py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"));

	py::class_<QuadModelPal, IModel, std::shared_ptr<QuadModelPal>>(mainModule, "QuadModelPal");
		//.////def(py::init<IBatch*, int, int, int, int, int>(),
		//	py::arg("batchId"), py::arg("x"), py::arg("y"), py::arg("w"), py::arg("h"), py::arg("pal"));

	py::class_<Node, std::shared_ptr<Node>>(mainModule, "Node")
		.def(py::init<>())
		.def("add", &Node::add, py::arg("child"))
		.def("setModel", &Node::setModel, py::arg("model"))
		.def("setPosition", [](Node &self, float x, float y, float z) {
			self.setPosition(glm::vec3(x, y, z));
		}, py::arg("x"), py::arg("y"), py::arg("z"));

}
