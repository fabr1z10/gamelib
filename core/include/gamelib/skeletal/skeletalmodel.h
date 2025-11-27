#pragma once

#include "../model.h"
#include "jointtransform.h"
#include "mesh.h"
#include "skeletalanimation.h"

struct JointInfo {
    JointInfo() : id(-1), parent(-1), scale(1.f), z(0.f), mesh(nullptr) {}
    JointInfo(int id, int parent, const std::string& name, glm::ivec3 weightIndex) : id(id),
        parent(parent), name(name), mesh(nullptr), weightIndex(weightIndex), scale(1.0f), z(0.0f) {}
    int id;
    int parent;
    float scale;
    float z;
    std::vector<int> children;
    std::string name;
    JointTransform restTransform;
    std::shared_ptr<Mesh> mesh;
    glm::ivec3 weightIndex;
};


class SkeletalModel : public IModel  {
public:
    // every skeletal model has by default a root joint (0)
	SkeletalModel(const std::string& camId);

    int addJoint(const std::string& joint, int parent, glm::ivec2 position);

	void setAnimation(const std::string&) override;

    void setMesh(int jointId, std::shared_ptr<Mesh>);

    void addAnimation(const std::string& animId, std::shared_ptr<SkeletalAnimation>);

    std::string getDefaultAnimation() const;

    void setDefaultAnimation(const std::string&);

	//void setVisible(bool) override {}
    //int getPrimitiveCount() const override {return 0;}
//
    void init();
    std::vector<glm::mat4> calculateCurrentPose(std::unordered_map<int, JointTransform>& pose);
    const std::vector<std::pair<int, glm::vec3>>& getOffsetPoints() const;
    int getJointId(const std::string&);
    //std::shared_ptr<IRenderer> getRenderer(int batchId) override;
    //const std::vector<std::shared_ptr<Mesh>>& getModels() const;
    SkeletalAnimation* getAnimation(const std::string& id);
    /**
     *
     * Gets an array of the all important model-space transforms of all the
     * joints (with the current animation pose applied) in the entity. The
     * joints are ordered in the array based on their joint index. The position
     * of each joint's transform in the array is equal to the joint's index.
     *
     * @return The array of model-space transforms of the joints in the current
     *         animation pose.
     */
    const glm::mat4& getRestTransform(int id) const;
    glm::ivec3 getWeightIndex(int id) const;
    const JointInfo& getJointInfo(int id) const;

    size_t getJointCount() const;

    bool hasCollision(const std::string & anim) const;

    //int getShaderId() const;
    //std::shared_ptr<Model> generateDebugModel();

    std::pair<int, int> getDebugShape(const std::string& anim, int attack);

	void draw(IShader*) override;
	void update() override ;

private:
	std::unordered_map<int, JointTransform> interpolatePoses(
			KeyFrame* previousFrame, KeyFrame* nextFrame, float progression);

	//std::shared_ptr<Shape> getRect(int mode, int, int, int, int);

    void computeOffset();
    std::vector<JointInfo> _jointInfos;
    std::vector<glm::mat4> _invRestTransforms2;
    std::vector<glm::mat4> _restTransforms2;

    std::vector<std::pair<std::string, std::string>> m_offsetPointIds;
    std::vector<std::pair<int, glm::vec3>> m_offsetPoints;

    // this is just a map from joint name to joint id
    std::unordered_map<std::string, int> _jointNameToId;

    std::unordered_map<std::string, std::shared_ptr<SkeletalAnimation>> _animations;

    std::string _defaultAnimation;

    // collision shapes (if any)
    //std::vector<std::shared_ptr<Shape>> m_shapes;

    std::unordered_map<std::string, std::vector<int>> m_animShapes;
    //Bounds m_staticBounds;
    std::vector<std::pair<int, int>> m_shapeInfo;
    float m_halfThickness;



	// current animation
	SkeletalAnimation* _currentAnimation;
	float _animationTime;
	std::vector<glm::mat4> _bones;
	IShader* _shader;
	int _modelLocation;
	int _viewLocation;
	int _projLocation;
	int _boneLocation;
	int _l2mLocation;
	int _weightIndexLocation;
	int _zLocation;
	Camera* _cam;
	std::string _camId;

};

//inline int SkeletalModel::getShaderId() const {
//    return _shaderId;
//}
inline const JointInfo & SkeletalModel::getJointInfo(int id) const {
    return _jointInfos[id];
}


inline size_t SkeletalModel::getJointCount() const {
    return _jointInfos.size();
}

inline const std::vector<std::pair<int, glm::vec3>>& SkeletalModel::getOffsetPoints() const {
    return m_offsetPoints;
}

inline int SkeletalModel::getJointId(const std::string & id) {
    auto i = _jointNameToId.find(id);
    if (i == _jointNameToId.end()) {
        return -1;
    }
    return i->second;
}



//inline Bounds SkeletalModel::getStaticBounds() const {
//    return m_staticBounds;
//}

inline std::string SkeletalModel::getDefaultAnimation() const {
    return _defaultAnimation;
}

inline void SkeletalModel::setDefaultAnimation(const std::string & anim) {
    _defaultAnimation = anim;
}