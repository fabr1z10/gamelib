#pragma once

#include <vector>
#include <string>
#include <unordered_map>

struct QuadInfo {
	//QuadInfo(int x, int y, int w, int h, int pal = 0, bool flipx = false, bool flipy = false) :
	//	x(x), y(y), width(w), height(h), palette(pal) {}

	float tx0 = 0.f, ty0 = 0.f;
	float tx1 = 0.f, ty1 = 0.f;
	// top left pixel coords
	int x = 0, y = 0;
	// quad size in pixels
	int width = 0, height = 0;
	int anchorX = 0;
	int anchorY = 0;
	int palette = 0;
	bool flipx = false;
	bool flipy = false;
};

struct Frame {
	QuadInfo quad;
	int ticks;
};

struct Animation {
	std::vector<Frame> frames;
	int loopIndex = 0;
};

class SpriteInfo {
public:
	SpriteInfo() = default;

	void addAnimation(const std::string& id, const Animation& anim);

	const Frame& getFrame(const std::string& animId, int frameIndex) const;

	int next(const std::string& animId, int currentFrame) const;

	std::string getDefaultAnimation() const;
private:
	std::string _defaultAnimation;

	std::unordered_map<std::string, Animation> _animations;
};

inline std::string SpriteInfo::getDefaultAnimation() const {
	return _defaultAnimation;
}