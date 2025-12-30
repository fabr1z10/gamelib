#pragma once

#include <string>
#include "gamelib/action.h"

namespace agi {

	class AGIRoom;

	class AGIAction : public Action {
	public:
		AGIAction() = default;

		void start() override;
	protected:
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

	class SetModel : public AGIAction {
	public:
		SetModel(const std::string& objectId, const std::string& modelId);

		void start() override;

		void run(double) override {}
	private:
		std::string _objectId;
		std::string _modelId;
	};



}