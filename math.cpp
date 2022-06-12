#include "SSS/Line/math.h"

glm::mat3 rotation_matrix2D_ccw(double angle)
{
	return glm::mat3(	glm::vec3(glm::cos(angle), -glm::sin(angle), 0), 
						glm::vec3(glm::sin(angle), glm::cos(angle), 0),
						glm::vec3(0, 0, 1) );
}

glm::mat3 rotation_matrix2D_cw(double angle)
{
	return glm::mat3(	glm::vec3(glm::cos(angle), glm::sin(angle), 0),
						glm::vec3(-glm::sin(angle), glm::cos(angle), 0),
						glm::vec3(0, 0, 1) );
}

glm::vec3 direction_vector2D(glm::vec3 v1, glm::vec3 v2)
{
	glm::vec2 a(v1.x, v1.y);
	glm::vec2 b(v2.x, v2.y);
	return glm::vec3(glm::normalize(b - a), v1.z);
}


glm::vec3 ortho_vector(glm::vec3& v1, glm::vec3& v2)
{
	return glm::vec3(-direction_vector2D(v1, v2).y, direction_vector2D(v1, v2).x, v1.z);
}

float intersect_factor(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, glm::vec3& p4)
{
	float numerator = (p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x);
	float denumerator = (p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y);

	if (glm::abs(denumerator) < 1e-6) {
		//Return 1 when the two vectors are parallel
		//As this function is used only in a 3 points case, the two vectors are always connected
		//When the denumerator is equal to 0, it indicate that the two lines are parallel
		return 1;
	}
	return numerator / denumerator;
}

glm::vec3 intersection_point(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, glm::vec3& p4)
{
	float factor = intersect_factor(p1, p2, p3, p4);

	float px = p1.x + factor * (p2.x - p1.x);
	float py = p1.y + factor * (p2.y - p1.y);

	return glm::vec3(px, py, p1.z);
}

float incidence_angle(glm::vec3 v1, glm::vec3 v2)
{
	return glm::acos( std::clamp( glm::dot(v1, v2) / (glm::length(v1) * glm::length(v2)) , -1.0f, 1.0f ) );
}



