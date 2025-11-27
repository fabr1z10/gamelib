//#pragma once
//
//#include "../renderer.h"
//
//#include "skeletalmodel.h"
//#include "skeletalanimation.h"
//#include "jointtransform.h"
//#include <vector>
//
//class SkeletalModel;
//
//struct Pose {
//    std::vector<glm::mat4> bonesTransform;
//    glm::mat4 offset;
//};
//
//class SkeletalRenderer : public IRenderer {
//public:
//    SkeletalRenderer(SkeletalModel* model) : _skeletalModel(model), _currentAnimation(nullptr) {}
//    void start() override ;
//    void updateGeometry() override {}
//    void update() override;
//    void draw(Shader*) override;
//    float getAnimationTime() const;
//    SkeletalModel* getSkeletalModel();
//    const std::vector<glm::mat4>& getBonesTransform();
//    Pose computePose(const std::string& anim, float t);
//
//private:
//    std::unordered_map<int, JointTransform> interpolatePoses(
//            KeyFrame* previousFrame, KeyFrame* nextFrame, float progression);
//
//    SkeletalModel* _skeletalModel;
//    SkeletalAnimation* _currentAnimation;
//    //std::string m_currentAnimationId;
//    float _animationTime;
//    std::vector<glm::mat4> _bones;
//    Camera* _cam;
//};
//
//inline const std::vector<glm::mat4> & SkeletalRenderer::getBonesTransform() {
//    return _bones;
//}
//
//inline SkeletalModel * SkeletalRenderer::getSkeletalModel() {
//    return _skeletalModel;
//}
//
//inline float SkeletalRenderer::getAnimationTime() const {
//    return _animationTime;
//}