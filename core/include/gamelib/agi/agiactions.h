#pragma once

#include <string>
#include "gamelib/action.h"
#include "gamelib/algo/gridgraph.h"
#include "gamelib/roomfactory.h"

class IModel;

namespace agi {

	class AGIRoom;

	class AGICharacter;

	class AGIAction : public Action {
	public:
		AGIAction() = default;

		void start() override;
	protected:
		IRoomFactory* _factory;
		AGIRoom* _room;

	};

	class Print : public AGIAction {
	public:
		Print(const std::string &message);

		void start() override;

		void run(double) override;
	private:
		std::string _msgId;
	};
//
//	class SetModel : public AGIAction {
//	public:
//		SetModel(const std::string& objectId, const std::string& modelId);
//
//		void start() override;
//
//		void run(double) override {}
//	private:
//		std::string _objectId;
//		std::string _modelId;
//	};
//
//	class Animate : public AGIAction {
//	public:
//		Animate(const std::string& objectId, const std::string& animationId, bool wait);
//
//		void start() override;
//
//		void run(double) override;
//	private:
//		std::string _objectId;
//		std::string _animationId;
//		bool _wait;
//		IModel * _model;
//	};
//
//	class Walk : public AGIAction {
//	public:
//		Walk(const std::string& objectId, const std::vector<Point> path);
//
//		void start() override;
//
//		void run(double) override;
//	private:
//		std::string _objectId;
//		const std::vector<Point> _path;
//		AGICharacter* _object;
//		int _index;
//	};
//

}