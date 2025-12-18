#include "helper.h"

#include <pybind11/pybind11.h>
#include <filesystem>
#include "gamelib/skeletal/skeletalmodel.h"
#include "gamelib/skeletal/skeletalanimation.h"

namespace py = pybind11;

std::string detectGameDirectory() {
	py::object main = py::module::import("__main__");

	if (!py::hasattr(main, "__file__")) {
		throw std::runtime_error("Cannot detect game folder: main.py not executed?");
	}

	std::string mainFile = main.attr("__file__").cast<std::string>();
	return std::filesystem::path(mainFile).parent_path().string();
}


void exportSkeletal(py::module_& module) {
	py::class_<SkeletalModel, IModel, std::shared_ptr<SkeletalModel>>(module, "SkeletalModel")
		.def(py::init<const std::string&>(), py::arg("camera_id"))
		.def("addJoint", &SkeletalModel::addJoint,
			 py::arg("name"), py::arg("parent"), py::arg("position"), py::arg("scale") = 1.f, py::arg("z") = 0.f)
		.def("setMesh", &SkeletalModel::setMesh,
			 py::arg("joint_id"), py::arg("mesh"))
		.def("setAnimation", &SkeletalModel::setAnimation,
			 py::arg("animation_id"))
		.def("addAnimation", &SkeletalModel::addAnimation,
			 py::arg("animation_id"), py::arg("animation"))
		.def("getDefaultAnimation", &SkeletalModel::getDefaultAnimation)
		.def("setDefaultAnimation", &SkeletalModel::setDefaultAnimation,
			 py::arg("animation_id"))
		.def("init", &SkeletalModel::init)
		.def("getJointId", &SkeletalModel::getJointId,
			 py::arg("joint_name"))
		.def("getAnimation", &SkeletalModel::getAnimation,
			 py::arg("animation_id"), py::return_value_policy::reference)
		.def("addOffsetPoint", &SkeletalModel::addOffsetPoint,
			 py::arg("joint_id"), py::arg("keypoint_id"));

	py::class_<SkeletalAnimation, std::shared_ptr<SkeletalAnimation>>(module, "SkeletalAnimation")
		.def(py::init<const std::string&>(), py::arg("filename"))
		.def("getLength", &SkeletalAnimation::getLength)
		.def("addKeyFrame", &SkeletalAnimation::addKeyFrame,
			 py::arg("keyframe"))
		.def("getKeyFrames", &SkeletalAnimation::getKeyFrames,
			 py::return_value_policy::reference)
		.def("getPreviousAndNextKeyFrames", &SkeletalAnimation::getPreviousAndNextKeyFrames,
			 py::arg("time"));

	py::class_<Mesh, std::shared_ptr<Mesh>>(module, "Mesh")
		.def(py::init<const std::string&, const std::string&>(),
			 py::arg("filename"), py::arg("mesh_id"));
}