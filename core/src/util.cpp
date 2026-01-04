#include "gamelib/util.h"
#include <stdexcept>


void SpriteInfo::addAnimation(const std::string&id, const Animation &anim) {
	if (_animations.empty()) {
		_defaultAnimation = id;
	}
	_animations[id] = anim;
}

int SpriteInfo::next(const std::string &animId, int currentFrame, bool& loopEnd) const {
	loopEnd = false;
	try {
		const Animation& anim = _animations.at(animId);
		int nextFrame = currentFrame + 1;
		if (nextFrame >= static_cast<int>(anim.frames.size())) {
			if (anim.loopIndex == -1) {
				nextFrame = anim.frames.size() - 1;
			} else {
				nextFrame = anim.loopIndex;
			}
			loopEnd = true;
		}
		return nextFrame;
	} catch (...) {
		throw std::runtime_error("SpriteInfo::next: animation id '" + animId +
								 "' not found.");
	}
}

const Frame& SpriteInfo::getFrame(const std::string&animId, int frameIndex) const {
	try {
		return _animations.at(animId).frames[frameIndex];
	} catch (...) {
		throw std::runtime_error("SpriteInfo::getFrame: animation id '" + animId +
								 "' or frame index " + std::to_string(frameIndex) + " not found.");
	}
}