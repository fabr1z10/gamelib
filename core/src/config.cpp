#include "gamelib/config.h"
#include <yaml-cpp/yaml.h>
#include "gamelib/yaml_extension.h"

Config::Config(const std::string &filePath) {
	try {
		YAML::Node config = YAML::LoadFile(filePath);
		_deviceSize = YAML::read<glm::ivec2>(config, "device_size");
		_windowSize = YAML::read<glm::ivec2>(config, "window_size", _deviceSize);
		_deviceAspectRatio = static_cast<float>(_deviceSize.x) / _deviceSize.y;
		_title = YAML::read<std::string>(config, "title","Untitled game");
		_defaultTicks = YAML::read<int>(config, "default_ticks", 10);
		_mouseOn = YAML::read<bool>(config, "mouse_on", false);
		std::cout << "Device size: " << _deviceSize.x << "x" << _deviceSize.y << std::endl;
	} catch (const YAML::BadFile& e) {
		throw std::runtime_error(std::string("Could not open config file: ") + e.what());
	} catch (const YAML::Exception &e) {
		throw std::runtime_error(std::string("YAML parsing error: ") + e.what());
	} catch (const std::exception &e) {
		throw std::runtime_error(std::string("Error loading config: ") + e.what());
	}
}