#include "camera.h"
// Return Lookat
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

// Basic Movement
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == FORWARD)
		Position += Front * velocity;
	if (direction == BACKWARD)
		Position -= Front * velocity;
	if (direction == LEFT)
		Position -= Right * velocity;
	if (direction == RIGHT)
		Position += Right * velocity;
}

// 
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}
	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	Zoom -= (float)yoffset;
	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::get_world_space(glm::mat4 proj_mat, glm::mat4 view_mat)
{
	float x = 2.0f * (xpos / SCR_WIDTH) - 1.0f;
	float y = 2.0f * -(ypos / SCR_HEIGHT) + 1.0f; // Normalize


	glm::vec4 screen_pos = glm::vec4(x, -y, -1.0f, 1.0f);

	glm::mat4 projection = proj_mat * view_mat;
	glm::mat4 projection_inv = inverse(projection);
	glm::vec4 world_pos = projection_inv * screen_pos;

	w_xpos = world_pos.x / world_pos.w;
	w_ypos = world_pos.y / world_pos.w;
	w_zpos = world_pos.z / world_pos.w;

}