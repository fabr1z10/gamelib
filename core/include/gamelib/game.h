#pragma once

#include <memory>
#include <string>
#include "config.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "gamelib/shaderregistry.h"
#include "gamelib/roomfactory.h"
#include "gamelib/keylistener.h"
#include "gamelib/mouselistener.h"
#include <filesystem>

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
	IRoomFactory* getRoomFactory();
	void setRoomFactory(std::shared_ptr<IRoomFactory> factory);
	Room* getCurrentRoom();
	void setCurrentRoom(std::shared_ptr<Room>);
	std::filesystem::path getHomeDir() const;
	glm::vec4 getWindowViewport() const;
	void registerToKeyboardEvent(KeyListener*);
	void unregisterToKeyboardEvent(KeyListener*);
	void registerToMouseEvent(MouseListener*);
	void unregisterToMouseEvent(MouseListener*);
	// transforms screen coordinate into device coordinates
	glm::vec2 getDeviceCoordinates(glm::vec2);

private:
	static void WindowResizeCallback(GLFWwindow* win, int width, int height);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_pos_callback(GLFWwindow*, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow*, int, int, int);
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
	std::string _homeDir;
	std::unordered_set<KeyListener*> _keyboardListeners;
	std::unordered_set<MouseListener*> _mouseListeners;
	bool _endRoom;
	float _screenHeight;
	std::filesystem::path _homePath;

};

inline glm::vec4 Game::getWindowViewport() const {
	return _windowViewport;
}

inline const Config* Game::getConfig() const {
	return _config.get();
}

inline GLFWwindow *Game::getWindow() const {
	return _window;
}

inline std::filesystem::path Game::getHomeDir() const {
	return _homePath;
}

inline IRoomFactory* Game::getRoomFactory() {
	return _roomFactory.get();
}