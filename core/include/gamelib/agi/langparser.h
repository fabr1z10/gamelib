#pragma once

#include <vector>
#include <string>
#include <unordered_map>

namespace agi {

	struct Instruction {
		int id;
		std::vector<int> deps;
		std::string payload;
		bool isMacro = false;
		std::string macroName;
		std::string macroArgs;
		bool isEmpty() const {
			return !isMacro && payload.empty();
		}

		bool isEntry() const {
			return deps.size() == 1 && deps[0] == 0;
		}
	};

	std::ostream& operator<<(std::ostream& os, const Instruction& f) ;

	struct Macro {
		std::string name;
		std::vector<Instruction> items;
		void addInstruction(const Instruction& instr) {
			if (!instr.isEmpty())
				items.push_back(instr);
		}

	};

	using dict = std::unordered_map<std::string, std::string>;

	class LanguageParser {
	public:
		LanguageParser(const std::string& file);

		std::vector<Instruction> expandMacro(Macro& macro, const dict& dictionary = dict());

		//std::vector<Instruction> getMacro(const std::string& name, const std::unordered_map<std::string, std::string>& vars);

		Instruction parseInstruction(const std::string& line);

	private:
		dict stringToDict(const std::string& s);

		std::unordered_map<std::string, Macro> _macros;

		std::string substituteVars(const std::string& in, const std::unordered_map<std::string, std::string>& vars);

		// --- Parsing ---
		void parseFile(const std::string& filename);


	};
}