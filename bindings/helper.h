#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include "gamelib/static_batch.h"

namespace py = pybind11;

std::string detectGameDirectory();

template<typename V, typename P>
void bindStaticBatch(py::module_& m, const char* name) {
	using SB = StaticBatch<V, P>;
	py::class_<SB, IBatch, std::shared_ptr<SB>>(m, name)
		.def(py::init<IShader*, Camera*>())
		.def("setData", &SB::setData);
}

void exportSkeletal(py::module_& mainModule);