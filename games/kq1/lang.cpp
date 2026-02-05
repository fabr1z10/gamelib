#include "lang.h"
#include "gamelib/game.h"
#include <fstream>
#include <iostream>


std::ostream& operator<<(std::ostream& os, const Instruction& f) {
	os << f.id << " (";
	for (const auto& d : f.deps) os << d << ", ";
	os << "): " << f.payload << "\n";
	return os;
}

LanguageParser::LanguageParser() {
	auto p = Game::instance().getHomeDir() / "assets" / "script.gs";
	//std::ifstream file(p);
	//if (!file) {
	//	std::cout << "Unable to find " << p << "\n";
	//	return;
	//}
	parseFile(p);
	expandMacro("sequenza");
	exit(1);
}



struct ExpansionFrame {
	ExpansionFrame(Macro* m, int macroIndex, int idAtPreviousLevel, const std::vector<int> deps) :
		macro(m), level(macroIndex), idAtPreviousLevel(idAtPreviousLevel), deps(deps) {}
	Macro * macro;
	int instrIndex = 0;
	int level;
	std::vector<int> deps;
	int idAtPreviousLevel;
	//std::vector<int> _entryNodes; // TODO maybe not needed after all
	std::unordered_set<int> _exitNodes;
	std::unordered_map<int, std::vector<int>> _map;

};

struct ExpandedInstruction {
	size_t level;
	int id;
	std::vector<std::pair<size_t, int>> previous;
	std::string payload;
};


// Recursive macro expansion function
std::vector<Instruction> LanguageParser::expandMacro(const std::string& name) {
	std::vector<ExpansionFrame> expansionStack;
	std::vector<Instruction> result;
	expansionStack.push_back(ExpansionFrame(&_macros.at(name), 0, 0, {0}));
	int nextInstruction = 1;
	int nextMacro = 1;
	while (!expansionStack.empty()) {
		ExpansionFrame& current = expansionStack.back();
		if (current.instrIndex >= current.macro->items.size()) {
			// finished this macro
			// when I pop, I need to map the local id to the exit nodes
			int localId = current.idAtPreviousLevel;
			std::vector<int> globalExitNodes;
			for (const auto& exit : current._exitNodes) {
				for (const auto& u : current._map.at(exit)) globalExitNodes.push_back(u);
			}
			expansionStack.pop_back();
			if (!expansionStack.empty()) {
				expansionStack.back()._map[localId] = globalExitNodes;
			}
			continue;
		}
		while (current.instrIndex < current.macro->items.size()) {
			const auto &instr = current.macro->items[current.instrIndex];
			// TODO I thought I might need this but actually not - restore it if really needed
			//if (instr.isEntry()) {
				// entry node
			//	current._entryNodes.push_back(instr.id);
			//}
			current._exitNodes.insert(instr.id);
			for (const auto &p: instr.deps) {
				current._exitNodes.erase(p);
			}

			if (instr.isMacro) {
				// push new frame
				current.instrIndex++;
				std::vector<int> gdeps;
				for (int k : instr.deps) {
					const auto& v = current._map.at(k);
					gdeps.insert(gdeps.end(), v.begin(), v.end());
				}
				expansionStack.push_back(ExpansionFrame(&_macros.at(instr.macroName), nextMacro++, instr.id, gdeps));
				break;
			} else {
				std::vector<int> globalPrevious;
				if (instr.isEntry()) {
					globalPrevious = current.deps;
				} else {
					for (const auto &p: instr.deps) {
						for (const auto &u: current._map.at(p)) {
							globalPrevious.push_back(u);
						}
					}
				}
				current._map[instr.id].push_back(nextInstruction);
				result.push_back({nextInstruction++, globalPrevious, instr.payload, false, ""});
				current.instrIndex++;
			}
		}

	}


	for (const auto& e : result) {
		std::cout << e;
	}
	return result;
}






static std::vector<int> parseDeps(const std::string& s)
{
	std::vector<int> out;
	std::stringstream ss(s);
	std::string tok;
	while (std::getline(ss, tok, ',')) {
		if (!tok.empty())
			out.push_back(std::stoi(tok));
	}
	return out;
}

static Instruction parseInstruction(const std::string& line)
{
	auto colon = line.find(':');
	if (colon == std::string::npos)
		throw std::runtime_error("Invalid instruction line: " + line);
	auto left  = line.substr(0, colon);
	auto right = line.substr(colon + 1);

	auto comma = left.find(',');
	int id = std::stoi(left.substr(0, comma));
	auto deps = parseDeps(left.substr(comma + 1));

	Instruction instr;
	instr.id = id;
	instr.deps = deps;

	// trim leading spaces
	right.erase(0, right.find_first_not_of(" \t"));

	if (!right.empty() && right[0] == '*') {
		instr.isMacro = true;
		instr.macroName = right.substr(1);
	} else {
		instr.payload = right;
	}

	return instr;
}


// Parse the file into a map of macros
void LanguageParser::parseFile(const std::string& filename) {
	std::ifstream file(filename);
	if (!file)
		throw std::runtime_error("Cannot open file: " + filename);

	Macro* current = nullptr;
	std::string line;

	while (std::getline(file, line)) {
		// remove leading/trailing whitespace
		line.erase(0, line.find_first_not_of(" \t"));
		line.erase(line.find_last_not_of(" \t") + 1);

		if (line.empty()) continue;

		if (line[0] == '@') {
			// New macro
			std::string name = line.substr(1);
			_macros[name] = Macro{name, {}};
			current = &_macros[name];
		} else {
			if (!current)
				throw std::runtime_error("Instruction outside of macro: " + line);

			current->items.push_back(parseInstruction(line));

		}
	}

}
