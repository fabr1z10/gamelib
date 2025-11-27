#pragma once

#include <vector>
#include <memory>
#include "gamelib/skeletal/keyframe.h"


class SkeletalAnimation {
public:
	SkeletalAnimation(const std::string& file);
    SkeletalAnimation();

    /**
     * @return The length of the animation in seconds.
     */
    float getLength();

    void addKeyFrame(std::shared_ptr<KeyFrame>);

    const std::vector<std::shared_ptr<KeyFrame>>& getKeyFrames();

    std::tuple<KeyFrame*, KeyFrame*, float> getPreviousAndNextKeyFrames (float t);

    const std::vector<std::pair<float, float>>& getAttacks() const;
    //std::unordered_map<std::string, glm::mat4> getAnimTransform(float, SkModel*);
private:

    float m_length;
    std::vector<std::shared_ptr<KeyFrame>> _keyFrames;
};


inline const std::vector<std::shared_ptr<KeyFrame>>& SkeletalAnimation::getKeyFrames() {
    return _keyFrames;
}

