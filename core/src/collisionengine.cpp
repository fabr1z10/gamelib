#include "gamelib/collisionengine.h"
#include "gamelib/node.h"

RaycastResult SpatialHashingCollisionEngine::raycastAxis(glm::vec2 origin, float length, int mask, Axis axis, Node *self) {
	int ix0, ix1, iy0, iy1;

	if (axis == Axis::Y) {
		int ix = floor(origin.x / _cellSize);
		iy0 = floor(origin.y / _cellSize);
		iy1 = floor((origin.y + length) / _cellSize);
		ix0 = ix1 = ix;      // fixed column
	} else {
		int iy = floor(origin.y / _cellSize);
		ix0 = floor(origin.x / _cellSize);
		ix1 = floor((origin.x + length) / _cellSize);
		iy0 = iy1 = iy;      // fixed row
	}

	int inc = length > 0 ? 1 : -1;

	int cellsToCheck = 1 + std::abs((axis == Axis::Y ? iy1 : ix1) -
									(axis == Axis::Y ? iy0 : ix0));

	std::unordered_set<Collider*> checked;

	for (int i = 0; i < cellsToCheck; ++i)
	{
		int ix = (axis == Axis::Y) ? ix0 : (ix0 + i * inc);
		int iy = (axis == Axis::Y) ? (iy0 + i * inc) : iy0;

		auto it = _colliders.find({ix, iy});
		if (it == _colliders.end()) continue;

		for (auto &c : it->second.colliders) {
			if ((c->getCollisionFlag() & mask) == 0) continue;
			if (c->getNode() == self || checked.count(c)) continue;
			checked.insert(c);

			// convert origin → collider-local
			glm::vec2 Ol = c->getNode()->worldToLocal(origin);

			// call proper raycast depending on axis
			auto result = c->getShape()->raycastAxis(Ol, length, axis);

			if (result) {
				return result;
			}
		}
	}

	return {};

}


SpatialHashingCollisionEngine::SpatialHashingCollisionEngine(float size) : ICollisionEngine(), _cellSize(size) {}

glm::ivec2 SpatialHashingCollisionEngine::hashPosition(glm::vec2 P) {
	return {floor(P.x / _cellSize), floor(P.y / _cellSize)};
}

/*
 * If I add a collider, I need to:
 * - compute which cells it occupies
 * - add it to those cells
 * - register as observer to the collider's node, so I can update its position when it moves
 * - store a mapping from node to collider for that purpose
 */
void SpatialHashingCollisionEngine::addCollider(Collider * collider) {
	auto b = collider->getBounds();
	auto im = hashPosition({b.xm, b.ym});
	auto iM = hashPosition({b.xM, b.yM});
	_colliderLocations.insert(std::make_pair(collider, ColliderLocation(im.x, iM.x, im.y, iM.y)));

	for (int i = im.x; i <= iM.x; ++i) {
		for (int j = im.y; j <= iM.y; ++j) {
			_colliders[{i, j}].add(collider);
		}
	}
	auto* node = collider->getNode();
	node->registerObserver(this);
	_nodeToCollider[node] = collider;
}

void SpatialHashingCollisionEngine::rmCollider(Collider * c) {
	auto& loc = _colliderLocations.at(c);
	for (int i = loc.xm; i <= loc.xM; ++i) {
		for (int j = loc.ym; j <= loc.yM; ++j) {
			_colliders.at({i, j}).colliders.erase(c);
		}
	}
	_colliderLocations.erase(c);
	_nodeToCollider.erase(c->getNode());

}

void SpatialHashingCollisionEngine::onRemove(Node * node) {
	rmCollider(_nodeToCollider.at(node));
}

void SpatialHashingCollisionEngine::onMove(Node * node) {
	auto* collider = _nodeToCollider.at(node);
	auto b = collider->getBounds();
	auto im = hashPosition({b.xm, b.ym});
	auto iM = hashPosition({b.xM, b.yM});
	auto& currentLocation = _colliderLocations.at(collider);
	if (currentLocation.xm == im.x && currentLocation.xM == iM.x &&
		currentLocation.ym == im.y && currentLocation.yM == iM.y) {
		// nothing to do here
		currentLocation.dirty = true;
		return;
	}
	// if we get here location has changed, so remove current location
	for (int i = currentLocation.xm; i <= currentLocation.xM; ++i) {
		for (int j = currentLocation.ym; j <= currentLocation.yM; ++j) {
			_colliders.at({i, j}).colliders.erase(collider);
		}
	}
	_colliderLocations[collider] = ColliderLocation(im.x, iM.x, im.y, iM.y);
	for (int i = im.x; i <= iM.x; ++i) {
		for (int j = im.y; j <= iM.y; ++j) {
			_colliders[{i, j}].add(collider);
		}
	}
}