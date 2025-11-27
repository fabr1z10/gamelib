#include "gamelib/skeletal/keyframe.h"

void KeyFrame::addJointInfo(const std::string &id, glm::vec3 position, float angle) {
    JointTransform tr;
    tr.translation = position;
    tr.rotation = glm::angleAxis(glm::radians(angle), glm::vec3(0.f, 0.f, 1.f));
    tr.scale = glm::vec3(1.f);
    _pose[id] = tr;

}
