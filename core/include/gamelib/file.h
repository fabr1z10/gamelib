#pragma once
#include <string>

#include <iostream>


class File {

public:
	explicit File(const std::string&);

	~File();

	bool good() const;

	const FILE* get() const noexcept {
		return _fp;
	}

	FILE* get() noexcept {
		return _fp;
	}

	// disable copy, allow move
	File(const File&) = delete;
	File& operator=(const File&) = delete;

	File(File&& other) noexcept;
	File& operator=(File&& other) noexcept;
private:
	FILE* _fp;
};

inline bool File::good() const {
	return _fp != nullptr;
}