#include <pybind11/pybind11.h>
#include <gamelib/game.h>
#include <gamelib/shader.h>
#include "pyroomfactory.h"

namespace py = pybind11;

PYBIND11_MODULE(gamelib, mainModule) {
	py::class_<Game>(mainModule, "Game", py::dynamic_attr())
		.def_static("instance", &Game::instance, py::return_value_policy::reference)
		.def("getShader", &Game::getShader, py::arg("id"), py::return_value_policy::reference)
		.def("init", &Game::init, py::arg("homeDir"))
		.def("setRoomFactory", &Game::setRoomFactory, py::arg("factory"), py::keep_alive<1, 2>())
		.def("run", &Game::run);

	py::class_<IShader>(mainModule, "Shader", py::dynamic_attr())
		.def("getBatch", &IShader::createBatch);

	py::class_<IRoomFactory, PyRoomFactory, std::shared_ptr<IRoomFactory>>(mainModule, "RoomFactory")
		.def(py::init<>())
		.def("createRoom", &IRoomFactory::createRoom);

	py::class_<Room, std::shared_ptr<Room>>(mainModule, "Room")
		.def(py::init<>())
		.def("setClearColor", [](Room &self, int r, int g, int b) {
			self.setClearColor(glm::ivec3{r, g, b});
		}, py::arg("r"), py::arg("g"), py::arg("b"));
}