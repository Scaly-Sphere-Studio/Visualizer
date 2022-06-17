#pragma once

#include "_includes.hpp"

//Return the sign of the number as 
// 1 for positives
// 0 for 0
//-1 for negatives
template <typename T> inline constexpr
int signum(T x, std::false_type) {
	return T(0) < x;
}

template <typename T> inline constexpr
int signum(T x, std::true_type) {
	return (T(0) < x) - (x < T(0));
}

template <typename T> inline constexpr
int signum(T x) {
	return signum(x, std::is_signed<T>());
}

//Linear interpolation between two values/points/vertices
template<class T>
T lerp(T v0, T v1, float t) {
	return (1 - t) * v0 + t * v1;
};

template<class T>
T smoothstep(T edge0, T edge1, float x)
{
    if (x < edge0) { return edge0; }
    if (x >= edge1) { return edge1; }

    // Scale/bias into [0..1] range
    x = (x - edge0) / (edge1 - edge0);

    return x * x * (3 - 2 * x);
};

template<class T>
T smootherstep(T edge0, T edge1, float x)
{
    if (x < edge0) { return edge0; }
    if (x >= edge1) { return edge1; }

    // Scale/bias into [0..1] range
    x = (x - edge0) / (edge1 - edge0);

    return x * x * x * (x * (x * 6 - 15) + 10);
};


// MATRIX
//Define the rotation matrix for a 90° anticlockwise rotation
const glm::mat3 rotation_matrix_90_ccw = glm::mat3(
    glm::vec3(0, -1, 0) ,
    glm::vec3(1, 0, 0),
    glm::vec3(0, 0, 1));
//Define the counter clockwise rotation 3D matrix for 2D space around the z axis
glm::mat3 rotation_matrix2D_ccw(double angle);
//Define the clockwise rotation matrix for 2D space
glm::mat3 rotation_matrix2D_cw(double angle);



// VECTORS
//Return the normalized colinear vector of two given points in 2D space
glm::vec3 direction_vector2D(glm::vec3 v1, glm::vec3 v2);

//Return the normalized orthogonal vector of two given points in 2D space with a 90° anticlockwise rotation  
glm::vec3 ortho_vector(glm::vec3& v1, glm::vec3& v2);


// INTERSECTION AND GEOMETRY
//Calculate the intermediate factor for the two lines intersection algorithm
//The factor give information about the natures of the lines relationship
float intersect_factor(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, glm::vec3& p4);
//Return the position of the intersection point between two lines
glm::vec3 intersection_point(glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, glm::vec3& p4);


// ANGLES
//Give the angle between two vectors
float incidence_angle(glm::vec3 v1, glm::vec3 v2);