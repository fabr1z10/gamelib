#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "gamelib/collision.h"
#include "gamelib/collider.h"
#include "gamelib/hashpair.h"
#include "gamelib/nodeobserver.h"


class Node;


class ICollisionEngine : public NodeObserver {
public:
	ICollisionEngine() = default;

	virtual ~ICollisionEngine() = default;

	virtual RaycastResult raycastAxis(
			glm::vec2 origin,
			float length,
			int mask,
			Axis axis,
			Node* self=nullptr) = 0;

	virtual void addCollider(Collider*) = 0;

	virtual void rmCollider(Collider*) = 0;

	virtual void checkCollisions() {}

	//virtual std::vector<Collider*> getColliders(int mask) const {}

	bool haveResponse(Collider*, Collider*);

	void addResponse(const std::string& tag1, const std::string& tag2, std::shared_ptr<CollisionResponse>);

	void onStart(Collider*, Collider*);

	void onEnd(Collider*, Collider*);


protected:
	std::unordered_map<std::pair<std::string, std::string>, std::shared_ptr<CollisionResponse>> _response;
};


class SpatialHashingCollisionEngine : public ICollisionEngine {
public:
	SpatialHashingCollisionEngine(float size);

	RaycastResult raycastAxis(
			glm::vec2 origin,
			float length,
			int mask,
			Axis axis,
			Node* self = nullptr) override;

	struct Cell {
		std::unordered_set<Collider*> colliders;
		bool dirty = true;

		void add(Collider* c) {
			colliders.insert(c);
			dirty = true;
		}
	};
	struct ColliderLocation {
		ColliderLocation() = default;
		ColliderLocation(int xm, int xM, int ym, int yM) : xm(xm), xM(xM), ym(ym), yM(yM), dirty(true) {}
		int xm, xM, ym, yM;
		bool dirty;
	};

	void addCollider(Collider* collider) override;

	void rmCollider(Collider* collider) override;

	void onMove(Node*) override;

	void onRemove(Node*) override;
private:
	float _cellSize;
	glm::ivec2 hashPosition(glm::vec2);
	std::unordered_map<glm::ivec2, Cell> _colliders;
	std::unordered_map<Collider*, ColliderLocation> _colliderLocations;
	std::unordered_map<Node*, Collider*> _nodeToCollider;

};