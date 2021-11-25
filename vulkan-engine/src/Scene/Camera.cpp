#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 eulerAngles, float fov, float aspect, float near, float far) : front(front),
fov(fov),
nearPlane(near),
farPlane(far),
aspect(aspect) {
	fov = fov / 57.2957795 + 0.2f;
	this->transform.setPosition(position);
	this->transform.setRotation(eulerAngles);
	this->up = glm::vec3(0, 1, 0);
	getFrontFromRotation();
	this->cameraUP = glm::normalize(glm::cross(right, front));
}

void Camera::getFrontFromRotation()
{
	glm::vec3 eulerAngles = transform.getEulerAngles();
	if (eulerAngles.x > 89) {
		eulerAngles.x = 89;
	}
	else if (eulerAngles.x < -89) {
		eulerAngles.x = -89;
	}

	glm::vec3 newFront;
	newFront.x = cos(glm::radians(eulerAngles.y)) * cos(glm::radians(eulerAngles.x));
	newFront.y = sin(glm::radians(eulerAngles.x));
	newFront.z = cos(glm::radians(eulerAngles.x)) * sin(glm::radians(eulerAngles.y));
	front = glm::normalize(newFront);
	right = glm::normalize(glm::cross(front, up));
}

glm::mat4 Camera::getViewMatrix()
{
	getFrontFromRotation();
	return glm::lookAt(this->transform.getPosition(), this->transform.getPosition() + front, this->up);
}

glm::mat4 Camera::getProjectionMatrix()
{
	glm::mat4 projection =  glm::perspective(glm::radians(80.0f), aspect, nearPlane, farPlane);
	projection[1][1] *= -1;
	return projection;
}
