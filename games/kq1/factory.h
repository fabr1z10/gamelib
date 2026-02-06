#include "gamelib/roomfactory.h"
#include "objinfo.h"
#include "lang.h"
class RoomFactory : public IRoomFactory {
public:
	RoomFactory();

	void createRoom() override;

	void init(std::shared_ptr<Room>) override;
private:
	std::unordered_map<std::string, std::shared_ptr<ObjInfo>> _objInfos;

	std::unordered_map<std::string, int> _state;

	//std::unique_ptr<LanguageParser> _parser;
};