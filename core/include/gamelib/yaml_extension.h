#pragma once

#include <yaml-cpp/yaml.h>
#include <glm/glm.hpp>
#include "gamelib/util.h"
#include "gamelib/agi/agiroom.h"

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

	template<>
	struct convert<QuadInfo> {
		static bool decode(const Node& node, QuadInfo& q) {
			// node must be a sequence
			if (!node.IsSequence()) return false;
			if (node.size() < 4) return false; // x,y,w,h mandatory

			q.x = node[0].as<int>();
			q.y = node[1].as<int>();
			q.width = node[2].as<int>();
			q.height = node[3].as<int>();
			q.anchorX = node[4].as<int>();
			q.anchorY = node[5].as<int>();

			// Optional 5th item: dictionary
			if (node.size() >= 7 && node[6].IsMap()) {
				const Node& opt = node[6];
				if (opt["pal"])    q.palette    = opt["pal"].as<int>();
				if (opt["flipx"])  q.flipx  = opt["flipx"].as<bool>();
				if (opt["flipy"])  q.flipy  = opt["flipy"].as<bool>();
			}

			return true;
		}
	};

	template <>
	struct convert<agi::ObjectInfo> {
		static bool decode(const Node& node, agi::ObjectInfo& obj) {
			if (!node.IsMap()) return false;
			if (node["room"])      obj.room      = node["room"].as<std::string>();
			if (node["model"])     obj.model     = node["model"].as<std::string>();
			if (node["x"])         obj.x         = node["x"].as<int>();
			if (node["y"])         obj.y         = node["y"].as<int>();
			return true;
		}
	};
} // namespace YAML