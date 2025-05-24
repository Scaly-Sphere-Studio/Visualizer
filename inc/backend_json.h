#pragma once
#include "commons.h"
#include "Box.h"

namespace glm {
	void to_json(nlohmann::json& j, const vec2& t);
	void from_json(const nlohmann::json& j, vec2& t);

	void to_json(nlohmann::json& j, const vec3& t);
	void from_json(const nlohmann::json& j, vec3& t);

	void to_json(nlohmann::json& j, const vec4& t);
	void from_json(const nlohmann::json& j, vec4& t);
}

//JSON CONVERTION
//Text data convertion

void to_json(nlohmann::json& j, const Box& t);
void from_json(const nlohmann::json& j, Box& t);

void to_json(nlohmann::json& j, Box::Shared const& t);
void from_json(const nlohmann::json& j, Box::Shared& t);
