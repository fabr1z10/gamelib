#include "gamelib/agi/langparser.h"
#include "gamelib/game.h"
#include <fstream>
#include "gamelib/agi/agiactions.h"
#include <iostream>

using namespace agi;

namespace agi {
	std::ostream &operator<<(std::ostream &os, const Instruction &f) {
		os << f.id << " (";
		for (const auto &d: f.deps) os << d << ", ";
		os << "): " << f.payload << "\n";
		return os;
	}
}

LanguageParser::LanguageParser(const std::string& file) {

	//auto p = Game::instance().getHomeDir() / "assets" / "script.gs";


	parseFile(file);
	expandMacro(_macros.at("sequenza"));//, dict());
	//exit(1);
}

dict LanguageParser::stringToDict(const std::string &s) {
	dict result;
	size_t i = 0;
	while (i < s.size()) {
		// Skip whitespace
		while (i < s.size() && std::isspace(s[i])) i++;

		// Read key
		size_t startKey = i;
		while (i < s.size() && s[i] != '=') i++;
		if (i == s.size()) break; // malformed
		std::string key = s.substr(startKey, i - startKey);

		i++; // skip '='

		// Read value
		std::string value;
		if (i < s.size() && s[i] == '"') {
			// Quoted value
			i++; // skip opening quote
			while (i < s.size()) {
				if (s[i] == '\\' && i+1 < s.size() && s[i+1] == '"') {
					value += '"';
					i += 2;
					continue;
				}
				if (s[i] == '"') {
					i++; // skip closing quote
					break;
				}
				value += s[i++];
			}
		} else {
			// Unquoted value, ends at next comma or end
			while (i < s.size() && s[i] != ',') {
				value += s[i++];
			}
		}

		// Add to map
		result[key] = value;

		// Skip comma
		if (i < s.size() && s[i] == ',') i++;
	}
	return result;
}

//std::vector<Instruction>
//LanguageParser::getMacro(const std::string &name, const std::unordered_map<std::string, std::string> &vars) {
////	auto instructions = expandMacro(name);
////	for (auto& instr : instructions) {
////		instr.payload = substituteVars(instr.payload, vars);
////	}
////	return instructions;
//
//}

std::string
LanguageParser::substituteVars(const std::string &in, const std::unordered_map<std::string, std::string> &vars) {
	// replace every occurrence of {x} with value of x
	std::string out;
	out.reserve(in.size());
	for (size_t i = 0; i < in.size();) {
		if (in[i] == '{') {
			size_t close = in.find('}', i + 1);
			if (close == std::string::npos) {
				throw std::runtime_error("Unmatched { in string: " + in);
			}
			std::string key = in.substr(i + 1, close - i - 1);
			auto it = vars.find(key);
			if (it == vars.end())
				throw std::runtime_error("Unknown variable {" + key + "} in: " + in);
			out += it->second;
			i = close + 1;
		} else {
			out += in[i++];
		}
	}
	return out;
}



struct ExpansionFrame {
	ExpansionFrame(Macro* m, const dict& args, int macroIndex, int idAtPreviousLevel, const std::vector<int> deps) :
			macro(m), arguments(args), level(macroIndex), idAtPreviousLevel(idAtPreviousLevel), deps(deps) {}
	Macro * macro;
	dict arguments;
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
std::vector<Instruction> LanguageParser::expandMacro(Macro& m, const dict& args) {
	std::vector<ExpansionFrame> expansionStack;
	std::vector<Instruction> result;
	expansionStack.push_back(ExpansionFrame(&m, args, 0, 0, {0}));
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
			std::vector<int> gdeps;
			if (instr.isEntry()) {
				gdeps = current.deps;
			} else {
				for (int k: instr.deps) {
					const auto &v = current._map.at(k);
					gdeps.insert(gdeps.end(), v.begin(), v.end());
				}
			}

			if (instr.isMacro) {
				// push new frame
				current.instrIndex++;
				auto da = stringToDict(substituteVars(instr.macroArgs, current.arguments));
				expansionStack.push_back(ExpansionFrame(&_macros.at(instr.macroName), da, nextMacro++, instr.id, gdeps));
				break;
			} else {
				current._map[instr.id].push_back(nextInstruction);
				result.push_back({nextInstruction++, gdeps, substituteVars(instr.payload, current.arguments), false, ""});
				current.instrIndex++;
			}
		}

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

Instruction LanguageParser::parseInstruction(const std::string& line)
{
	if (line.empty() || line[0] == '#') {
		return Instruction();
	}
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
		auto openBracket = right.find('(');
		if (openBracket == std::string::npos) {
			instr.macroName = right.substr(1);
		} else {
			instr.macroName = right.substr(1, openBracket - 1);
			instr.macroArgs = right.substr(openBracket + 1, right.find(')') - openBracket - 1);
			// TODO parse args
		}

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

			current->addInstruction(parseInstruction(line));

		}
	}

}

std::shared_ptr<Script> LanguageParser::getScript(const std::vector<Instruction> &instructions) {
	auto script = std::make_shared<Script>();
	for (const auto& instr : instructions) {
		std::cout << instr.payload << std::endl;
		auto bo = instr.payload.find('(');
		if (bo == std::string::npos) {
			auto cmd = instr.payload;
			// TODO handle command without args

		} else {
			auto args = instr.payload.substr(bo + 1, instr.payload.find(')') - bo - 1);
			auto cmd = instr.payload.substr(0, bo);
			auto dargs = stringToDict(args);
			if (cmd == "print") {
				script->addAction(instr.id, std::make_shared<agi::Print>(dargs.at("id")), instr.deps);
			}
		}

	}
	return  script;
}