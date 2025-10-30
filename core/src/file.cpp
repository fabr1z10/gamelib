#include "gamelib/file.h"
#include "gamelib/game.h"

File::File(const std::string & filename) : _fp(nullptr) {
	std::string fullName = Game::instance().getHomeDir() + "/" + filename;
	_fp = fopen(fullName.c_str(), "rb");

}

File::~File() {
	if (_fp != nullptr) {
		fclose(_fp);
		_fp = nullptr;
	}
}

File::File(File&& other) noexcept : _fp(other._fp) {
	other._fp = nullptr;
}

File& File::operator=(File&& other) noexcept {
	if (this != &other) {
		if (_fp) std::fclose(_fp);
		_fp = other._fp;
		other._fp = nullptr;
	}
	return *this;
}
