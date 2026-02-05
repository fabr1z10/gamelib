#pragma once

#include "gamelib/script.h"
#include <variant>

struct Instruction {
	int id;
	std::vector<int> deps;
	std::string payload;
	bool isMacro = false;
	std::string macroName;
	bool isEntry() const {
		return deps.size() == 1 && deps[0] == 0;
	}

};

std::ostream& operator<<(std::ostream& os, const Instruction& f) ;


struct Macro {
	std::string name;
	std::vector<Instruction> items;
};






class LanguageParser {
public:
	LanguageParser();

private:
	std::unordered_map<std::string, Macro> _macros;

	// --- Parsing ---
	void parseFile(const std::string& filename);
	std::vector<Instruction> expandMacro(const std::string& name);


};