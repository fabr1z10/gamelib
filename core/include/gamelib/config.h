#pragma once

#include <iostream>
#include <glm/glm.hpp>


class Config {
public:
	Config(const std::string& filePath);

	glm::ivec2 getDeviceSize() const;

	glm::ivec2 getWindowSize() const;

	std::string getTitle() const { return _title; }

	float getDeviceAspectRatio() const;

	int getDefaultTicks() const { return _defaultTicks; }

	bool mouseOn() const;
private:
	glm::ivec2 _deviceSize;
	glm::ivec2 _windowSize;
	std::string _title;
	int _defaultTicks;
	float _deviceAspectRatio;
	bool _mouseOn;
};

inline glm::ivec2 Config::getDeviceSize() const {
	return _deviceSize;
}

inline glm::ivec2 Config::getWindowSize() const {
	return _windowSize;
}

inline float Config::getDeviceAspectRatio() const {
	return _deviceAspectRatio;
}

inline bool Config::mouseOn() const {
	return _mouseOn;
}