#include "gamelib/roomfactory.h"
#include "objinfo.h"

class RoomFactory : public IRoomFactory {
public:
	RoomFactory();

	std::shared_ptr<Room> createRoom() override;

	void init(std::shared_ptr<Room>) override;
private:
	std::unordered_map<std::string, std::shared_ptr<ObjInfo>> _objInfos;

	std::unordered_map<std::string, int> _state;
};