#include <pybind11/pybind11.h>
#include "game.h"

namespace py = pybind11;

PYBIND11_MODULE(gamelib, mainModule) {
	py::class_<Game>(mainModule, "Game", py::dynamic_attr())
		.def_static("instance", &Game::instance, py::return_value_policy::reference);

}