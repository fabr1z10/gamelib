#pragma once

#include <string>
#include <unordered_map>
#include "jointtransform.h"
#include <glm/glm.hpp>

/**
 *
 * Represents one keyframe of an animation. This contains the timestamp of the
 * keyframe, which is the time (in seconds) from the start of the animation when
 * this keyframe occurs.
 *
 * It also contains the desired bone-space transforms of all of the joints in
 * the animated entity at this keyframe in the animation (i.e. it contains all
 * the joint transforms for the "pose" at this time of the animation.). The
 * joint transforms are stored in a map, indexed by the name of the joint that
 * they should be applied to.
 *
 * @author Karl
 *
 */
class KeyFrame {
public:
    KeyFrame (float t) : _timeStamp(t) {}

	KeyFrame(const KeyFrame&) = default;

    void addJointInfo(const std::string& id, glm::vec3 position, float angle) ;

    /**
	 * @return The time in seconds of the keyframe in the animation.
	 */
    float getTimeStamp() const;

	void setTimeStamp(float);
    const std::unordered_map<std::string, JointTransform>& getJointKeyFrames() const;
    int getIndex() const;
private:
    int _index;
    float _timeStamp;
    std::unordered_map<std::string, JointTransform> _pose;
};

inline int KeyFrame::getIndex() const {
    return _index;
}

inline float KeyFrame::getTimeStamp() const {
    return _timeStamp;
}

inline void KeyFrame::setTimeStamp(float value) {
	_timeStamp = value;
}

inline const std::unordered_map<std::string, JointTransform>& KeyFrame::getJointKeyFrames() const {
    return _pose;
}