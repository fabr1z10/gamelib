#pragma once

#include <string>
#include "gamelib/component.h"
#include "gamelib/shape.h"

class Collider : public Component {

public:
	Collider() = default;

	~Collider() override;

	Bounds getBounds();

	void start() override;

	virtual Shape* getShape() = 0;

	virtual int getCollisionFlag() const = 0;

	virtual int getCollisionMask() const = 0;

	virtual std::string getTag() const = 0;

};

class BasicCollider : public Collider {
public:
	BasicCollider(std::shared_ptr<Shape>, int flag, int mask, const std::string& tag);

	Shape* getShape() override;

	int getCollisionFlag() const override;

	int getCollisionMask() const override;

	std::string getTag() const override;
private:
	int _flag;
	int _mask;
	std::string _tag;
	std::shared_ptr<Shape> _shape;
};