#include "camera.h"
#include <cstdio>

mat4 rotateAbout(vec3 axis, float radians)
{
	mat4 matrix;

	matrix[0][0] = cos(radians) + axis.x*axis.x*(1-cos(radians));
	matrix[1][0] = axis.x*axis.y*(1-cos(radians))-axis.z*sin(radians);
	matrix[2][0] = axis.x*axis.z*(1-cos(radians)) + axis.y*sin(radians);

	matrix[0][1] = axis.y*axis.x*(1-cos(radians)) + axis.z*sin(radians);
	matrix[1][1] = cos(radians) + axis.y*axis.y*(1-cos(radians));
	matrix[2][1] = axis.y*axis.z*(1-cos(radians)) - axis.x*sin(radians);

	matrix[0][2] = axis.z*axis.x*(1-cos(radians)) - axis.y*sin(radians);
	matrix[1][2] = axis.z*axis.y*(1-cos(radians)) + axis.x*sin(radians);
	matrix[2][2] = cos(radians) + axis.z*axis.z*(1-cos(radians));

	return matrix;
}

Camera::Camera():	dir(vec3(0, 0, -1)), 
					right(vec3(1, 0, 0)), 
					up(vec3(0, 1, 0)),
					pos(vec3(0, 0, 0))
{}

Camera::Camera(vec3 _dir, vec3 _pos):dir(normalize(_dir)), pos(_pos)
{
	right = normalize(cross(_dir, vec3(0, 1, 0)));
	up =  normalize(cross(right, _dir));
}

/*
	[ Right 0 ]
	[ Up 	0 ]
	[ -Dir	0 ]
	[ 0 0 0 1 ]
*/

mat4 Camera::getMatrix()
{
	mat4 cameraRotation = mat4(
			vec4(right, 0),
			vec4(up, 0),
			vec4(-dir, 0),
			vec4(0, 0, 0, 1));

	mat4 translation = mat4 (
			vec4(1, 0, 0, 0),
			vec4(0, 1, 0, 0),
			vec4(0, 0, 1, 0),
			vec4(-pos, 1));

	return transpose(cameraRotation)*translation;
}

void Camera::rotateCamera(float x, float y)
{
	mat4 rotateAroundY = rotateAbout(vec3(0, 1, 0), x);
	mat4 rotateAroundX = rotateAbout(right, y);

	pos = (rotateAroundX*rotateAroundY*glm::vec4(pos,0));

	dir = normalize(rotateAroundX*rotateAroundY*vec4(dir, 0));
	right = normalize(cross(dir, vec3(0, 1, 0)));
	up =  normalize(cross(right, dir));
}
