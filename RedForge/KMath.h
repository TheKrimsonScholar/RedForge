#pragma once

#include <ostream>
#include <istream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

inline std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
	return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}
inline std::istream& operator>>(std::istream& is, glm::vec3& v)
{
	char filler;
	return is >> filler >> v.x >> filler >> v.y >> filler >> v.z >> filler;
}

inline std::ostream& operator<<(std::ostream& os, const glm::quat& q)
{
	return os << '(' << q.x << ", " << q.y << ", " << q.z << ", " << q.w << ')';
}
inline std::istream& operator>>(std::istream& is, glm::quat& q)
{
	char filler;
	return is >> filler >> q.x >> filler >> q.y >> filler >> q.z >> filler >> q.w >> filler;
}

class Math
{

};