#pragma once

#include <memory>
#include <string>
#include "config.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "gamelib/shaderregistry.h"
#include "gamelib/roomfactory.h"

class Game {
public:
	static Game& instance() {
		static Game instance;
		return instance;
	}
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;
	~Game() = default;
	void init(const std::string& homeDir);
	void run();
	const Config* getConfig() const;
	GLFWwindow* getWindow() const;
	IShader* getShader(const std::string& id);
	void setRoomFactory(std::shared_ptr<IRoomFactory> factory);
private:
	static void WindowResizeCallback(GLFWwindow* win, int width, int height);
	void initGL();
	void closeGL();
	Game();
	double _frameTime;
	double _timeLastUpdate;
	std::unique_ptr<Config> _config;
	glm::vec4 _windowViewport;
	GLFWwindow* _window;
	ShaderRegistry _shaderRegistry;
	std::shared_ptr<IRoomFactory> _roomFactory;
	std::shared_ptr<Room> _room;
};

inline const Config* Game::getConfig() const {
	return _config.get();
}

inline GLFWwindow *Game::getWindow() const {
	return _window;
}

