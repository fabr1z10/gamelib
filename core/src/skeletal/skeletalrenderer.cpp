//#include "skeletalrenderer.h"
//#include <glm/gtx/transform.hpp>
//
//
//
////void SkeletalRenderer::update() {
////    if (!_started) {
////        return;
////    }
////    auto dt = 1./60.;
////    std::unordered_map<int, JointTransform> pose;
////    if (_currentAnimation != nullptr) {
////        _animationTime += static_cast<float>(dt);
////        if (_animationTime >= _currentAnimation->getLength()) {
////            //m_complete = true;
////            //if (_currentAnimation->loop()) {
////            _animationTime = fmod(_animationTime, _currentAnimation->getLength());
////            //}  else {
////            //    _animationTime = _currentAnimation->getLength() - 0.001f;
////            //}
////        }
////        // compute current pose
////        auto frames = _currentAnimation->getPreviousAndNextKeyFrames(_animationTime);
////        pose = interpolatePoses(std::get<0>(frames), std::get<1>(frames), std::get<2>(frames));
////    }
////
////    _bones = _skeletalModel->calculateCurrentPose(pose);
////
////    // apply offset
//////    const auto& offsetPoints = _skeletalModel->getOffsetPoints();
//////    if (!offsetPoints.empty()) {
//////        glm::vec3 offset(0.0f);
//////        //std::cout << "no of offset points: " << offsetPoints.size() << "\n";
//////        for (const auto &a : offsetPoints) {
//////            // find coordinates of offset pointg
//////            glm::vec4 p = _bones[a.first] * glm::vec4(a.second, 1.0f);
//////            offset.y = std::max(-p.y, offset.y);
//////        }
//////        //std::cerr << offset.y << "\n";
//////        setTransform(glm::translate(offset));
//////    }
////
////}
//
//void SkeletalRenderer::start() {
//    _skeletalModel->init();
//    _started = true;
//    _cam = Game::instance().getRoom()->getCamera(0);
//    auto defaultAnim = _skeletalModel->getDefaultAnimation();
//    _currentAnimation = _skeletalModel->getAnimation(defaultAnim);
//    _animationTime = 0.f;
//}
//
//void SkeletalRenderer::draw(Shader * s) {
//
//    if (s->getId() == _skeletalModel->getShaderId()) {
//        auto modelLoc = glGetUniformLocation(s->getProgId(), "model");
//        auto viewLoc = glGetUniformLocation(s->getProgId(), "view");
//        auto projLoc = glGetUniformLocation(s->getProgId(), "projection");
//        auto w = _node->getWorldMatrix();
//        auto pm = _cam->getProjectionMatrix();
//        auto vm = _cam->getViewMatrix();
//        glUniformMatrix4fv(modelLoc, 1, false, &w[0][0]);
//        glUniformMatrix4fv(viewLoc, 1, false, &vm[0][0]);
//        glUniformMatrix4fv(projLoc, 1, false, &pm[0][0]);
//
//
//        auto boneLoc = glGetUniformLocation(s->getProgId(), "Bone");
//        auto l2m = glGetUniformLocation(s->getProgId(), "local_to_model");
//        auto weightIndex = glGetUniformLocation(s->getProgId(), "weightIndex");
//        auto pz = glGetUniformLocation(s->getProgId(), "z");
//        glUniformMatrix4fv(boneLoc, _bones.size(), false, &_bones[0][0][0]);
//
//        //for (const auto& model : _skeletalModel->getModels()) {
//        for (size_t n = 0; n < _skeletalModel->getJointCount(); ++n) {
//            const auto& jinfo = _skeletalModel->getJointInfo(n);
//
//            auto restTransform = _skeletalModel->getRestTransform(n);
//
//            auto weightIndices = jinfo.weightIndex;// m_skeletalModel->getWeightIndex(n++);
//            glUniformMatrix4fv(l2m, 1, false, &restTransform[0][0]);
//            glUniform3iv(weightIndex, 1, &weightIndices[0]);
//            glUniform1f(pz, jinfo.z);
//            if (jinfo.mesh != nullptr) {
//                jinfo.mesh->draw(s);
//            }
//        }
//    }
//}
//
//
////std::unordered_map<int, JointTransform> SkeletalRenderer::interpolatePoses(
////        KeyFrame* previousFrame, KeyFrame* nextFrame, float progression) {
////    std::unordered_map<int, JointTransform> currentPose;
////    const auto& nf = nextFrame->getJointKeyFrames();
////    for (const auto& p : previousFrame->getJointKeyFrames()) {
////        // the model might not have this joint ... in this case nothing to do
////        int jointId = _skeletalModel->getJointId(p.first);
////        if (jointId == -1) {
////            continue;
////        }
////
////        // previousTransform is p.second
////        JointTransform nextTransform = nf.at(p.first);
////        //JointTransform currentTransform = m_model->getRestTransform(p.first);
////        auto localTransform = JointTransform::interpolate(p.second, nextTransform, progression);
////        //currentTransform += JointTransform::interpolate(p.second, nextTransform, progression);
////        //currentTransform.z = 0;
////        //std::cout << m_animationTime << " . " << currentTransform.alpha << "\n";
////        currentPose.insert(std::make_pair(jointId, localTransform));
////    }
////    return currentPose;
////}