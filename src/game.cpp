#include "game.h"
#include <pybind11/pybind11.h>
#include <filesystem>
#include <iostream>


namespace py = pybind11;



std::unique_ptr<Game> Game::_instance = nullptr;


Game::Game() {
	std::string gameDir = detectGameDir(); // <- implemented below
	std::cout << "game directory: " << gameDir << std::endl;
	//loadSettings(gameDir + "/settings.yaml");

}

std::string Game::detectGameDir() {
	// simplest way: ask Python what `__main__`'s path is
	py::object sys = py::module::import("sys");
	py::object mainModule = sys.attr("modules")["__main__"];
	py::object mainFile = mainModule.attr("__file__");
	return std::filesystem::path(mainFile.cast<std::string>()).parent_path().string();
}