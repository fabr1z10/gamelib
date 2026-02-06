#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include "gamelib/agi/langparser.h"

namespace agi {

	class AGIRoom;

	class Trie {

	public:
		std::unordered_map<std::string, std::shared_ptr<Trie>> children;
		std::shared_ptr<Macro> macro;
	};

	class AGIParser {
	public:
		AGIParser(AGIRoom* room) :_room(room) {}

		virtual void parse(const std::string &input) = 0;

		std::vector<std::string> splitBySpaces(const std::string &input);

		virtual void addSaid(const std::vector<std::string> &words, std::shared_ptr<Macro> m) = 0;

	protected:
		AGIRoom *_room = nullptr;
	};

	class AGITokenParser : public AGIParser {
	public:

		AGITokenParser(AGIRoom* room, const std::string &file);

		void parse(const std::string &input) override;

		void addSaid(const std::vector<std::string> &words, std::shared_ptr<Macro> m) override;

	private:
		std::shared_ptr<Trie> _parserRoot;
		std::unordered_map<std::string, std::string> _groupMap;
	};
}