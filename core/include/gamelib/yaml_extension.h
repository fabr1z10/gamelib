#pragma once

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>

namespace YAML {

	template <typename T>
	T read(const YAML::Node& node, const std::string& key) {
		if (node[key]) {
			return node[key].as<T>();
		} else {
			std::string msg = "Failed to convert key '" + key + "' to requested type";
			throw std::runtime_error(msg);
		}
	}

	template <typename T>
	T read(const YAML::Node& node, const std::string& key, const T& defaultValue) {
		if (node[key]) {
			return node[key].as<T>();
		} else {
			return defaultValue;
		}
	}

	// Generic convert specialization for glm::tvec<N, T, P>
	template <typename T, glm::precision P, int N>
	struct convert<glm::vec<N, T, P>> {
		using VecType = glm::vec<N, T, P>;

		static Node encode(const VecType& rhs) {
			Node node;
			for (int i = 0; i < N; ++i)
				node.push_back(rhs[i]);
			return node;
		}

		static bool decode(const Node& node, VecType& rhs) {
			if (!node.IsSequence() || node.size() != N)
				return false;
			for (int i = 0; i < N; ++i)
				rhs[i] = node[i].as<T>();
			return true;
		}
	};

} // namespace YAML