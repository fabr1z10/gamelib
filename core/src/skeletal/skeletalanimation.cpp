#include "gamelib/skeletal/skeletalanimation.h"
#include "gamelib/game.h"
#include "../error.h"
#include <yaml-cpp/yaml.h>

SkeletalAnimation::SkeletalAnimation(const std::string &file) {
	auto assetPath = Game::instance().getHomeDir() / "assets";
	try {
		YAML::Node inputData = YAML::LoadFile(assetPath / file);
		auto duration = inputData["duration"].as<float>();
		for (const auto& kf : inputData["key_frames"]) {
			auto pct = kf["pct"].as<float>();
			auto keyFrame = std::make_shared<KeyFrame>(pct * duration);
			for (const auto& b : kf["bones"]) {
				auto boneId = b.first.as<std::string>();
				auto data = b.second.as<std::vector<float>>();
				keyFrame->addJointInfo(boneId, glm::vec3(data[0], data[1], data[2]), data[3]);
			}
			this->addKeyFrame(keyFrame);
		}
		auto endFrame = std::shared_ptr<KeyFrame>(new KeyFrame(*_keyFrames[0].get()));
		endFrame->setTimeStamp(duration);
		this->addKeyFrame(endFrame);



	} catch (const YAML::BadFile& e) {
		throw std::runtime_error("SkeletalAnimation: Unable to open file");
	}
}


void SkeletalAnimation::addKeyFrame(std::shared_ptr<KeyFrame> keyframe) {
    _keyFrames.push_back(keyframe);
    m_length = keyframe->getTimeStamp();
}

SkeletalAnimation::SkeletalAnimation() {
}



float SkeletalAnimation::getLength() {
    return m_length;
}

std::tuple<KeyFrame*, KeyFrame*, float> SkeletalAnimation::getPreviousAndNextKeyFrames(float t) {
    if (t < 0 || t > m_length) {
        return std::make_tuple(nullptr, nullptr, 0.0f);
    }
    for (int i = 1; i < _keyFrames.size(); ++i) {
        if (_keyFrames[i]->getTimeStamp() > t) {
            float progression = (t - _keyFrames[i-1]->getTimeStamp()) / (_keyFrames[i]->getTimeStamp() - _keyFrames[i-1]->getTimeStamp());
            return std::make_tuple(_keyFrames[i-1].get(), _keyFrames[i].get(), progression);
        }
    }
    // if we get here, we are in a loop
    float progression = (t - _keyFrames.back()->getTimeStamp()) / (m_length - _keyFrames.back()->getTimeStamp());
    return std::make_tuple(_keyFrames.back().get(), _keyFrames.front().get(), progression);

}
