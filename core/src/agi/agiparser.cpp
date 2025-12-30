#include "gamelib/agi/agiparser.h"
#include "yaml-cpp/yaml.h"
#include <sstream>
#include <iostream>
#include "gamelib/agi/agiroom.h"

using namespace agi;

std::vector<std::string> AGIParser::splitBySpaces(const std::string &str) {
	std::istringstream iss(str);
	std::vector<std::string> tokens;
	std::string word;
	while (iss >> word) { // automatically skips extra spaces
		tokens.push_back(word);
	}
	return tokens;
}

AGITokenParser::AGITokenParser(AGIRoom* room, const std::string &file) : AGIParser(room) {
	try {
		YAML::Node words = YAML::LoadFile(file);
		for (const auto& item : words["words"]) {
			std::string groupId = item.first.as<std::string>();
			for (const auto& wordNode : item.second) {
				std::string word = wordNode.as<std::string>();
				_groupMap[word] = groupId;
			}
		}
		_parserRoot = std::make_shared<Trie>();


	} catch (const YAML::BadFile &e) {
		throw std::runtime_error(std::string("Could not open room file: ") + e.what());
	} catch (const YAML::Exception &e) {
		throw std::runtime_error(std::string("YAML parsing error: ") + e.what());
	} catch (const std::exception &e) {
		throw std::runtime_error(std::string("Error loading room: ") + e.what());
	}
}

void AGITokenParser::addSaid(const std::vector<std::string> &words, const std::function<void()> &callback) {
	Trie* t = _parserRoot.get();
	for (const auto& w : words) {
		auto it = t->children.find(w);
		if (it == t->children.end()) {
			auto newNode = std::make_shared<Trie>();
			t->children[w] = newNode;
			t = newNode.get();
		} else {
			t = it->second.get();
		}
	}
	t->callbacks.push_back(callback);
}

void AGITokenParser::parse(const std::string &input) {
	// as a first step, I split the string into words separated by space
	auto words = splitBySpaces(input);
	// map words to group ids
	int iStart = 0;
	std::vector<std::string> gIds;
	while (iStart < words.size()) {
		int j = words.size();
		std::string gId;
		// try to find the longest matching group
		while (j > iStart) {
			std::string candidate;
			for (int k = iStart; k < j; ++k) {
				candidate += words[k] + " ";
			}
			candidate = candidate.substr(0, candidate.size() - 1); // remove last space
			auto it = _groupMap.find(candidate);
			if (it != _groupMap.end()) {
				gId = it->second;
				break;
			}
			j--;
		}
		if (gId.empty()) {
			// not found, use ANYWORD
			gId = "_ANY_";
			j = iStart + 1;
		}
		if (gId != "_IGNORE_") {
			gIds.push_back(gId);
		}
		iStart = j;
	}

	for (const auto& g : gIds) {
		std::cout << g  << "; ";
	}
	std::cout << "\n";

	int i = 0;
	std::string errWord;
	Trie* t = _parserRoot.get();
	while (i < gIds.size()) {
		auto it = t->children.find(gIds[i]);
		if (it == t->children.end()) {
			// not found, try anyword
			it = t->children.find("*");
			if (it == t->children.end()) {
				t = nullptr;
				break;
			}
		}
		// increase i
		i++;
		t = it->second.get();
	}
	if (t == nullptr) {
		auto culpritWord = words[i];

		std::cout << "Don't know " + culpritWord << "\n";
		_room->printMessage("I don't understand \"" + culpritWord + "\".");
		//print("I don't understand \"" + culpritWord + "\".");
	} else {
		std::cout << ("Found node!\n");
		if (!t->callbacks.empty()) {
			for (const auto& cb : t->callbacks) {
				cb();
			}
		}
	}


}