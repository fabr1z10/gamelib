#pragma once

#include "gamelib/component.h"

class ICollisionEngine;
class ShadowModelPal;

class Shadow : public Component {
public:
	Shadow(float angle, float scale, int palette);

	void start() override;

	void update(double) override;

private:
	ICollisionEngine* _collisionEngine;
	float _angle;
	float _scale;
	int _palette;
	ShadowModelPal* _sm;
};