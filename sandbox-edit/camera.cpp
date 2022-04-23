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
	tool_distance += (float)yoffset;
	if (tool_distance < 1.0f)
		tool_distance = 1.0f;
	if (tool_distance > 100.0f)
		tool_distance = 100.0f;
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
	float x = (2.0f * xpos) / SCR_WIDTH - 1.0f;
	float y = 1.0f - (2.0f * ypos) / SCR_HEIGHT; // Normalize


	glm::vec4 screen_pos = glm::vec4(x, y, -1.0f, 1.0f); 

	glm::mat4 projection_inv = inverse(proj_mat);
	glm::mat4 view_inv = inverse(view_mat);
	glm::vec4 ray_view = screen_pos * projection_inv;

	ray_view = glm::vec4(ray_view.x, ray_view.y, -1.0f, 0.0f);

	glm::vec4 world_pos = ray_view * view_inv;
	//float w = world_pos[3] * screen_pos[0] + world_pos[7] * y + world_pos[11] * z + m[15];
	//world_pos = glm::normalize(world_pos);
	//w_xpos = world_pos.x;
	//w_ypos = world_pos.y;
	//w_zpos = world_pos.z;
}

void Camera::ray_to_world()
{
	glm::vec3 start,end;
	ray new_ray;
	new_ray.origin = glm::vec3(Position);
	new_ray.dir = glm::vec3(Front);
	start = new_ray.origin;
	end = new_ray.origin + new_ray.dir * tool_distance;
	w_xpos = end.x;
	w_ypos = end.y;
	w_zpos = end.z;
}