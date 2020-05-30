#include "camera.hpp"

inline void Camera::revalidate_all()
{
	StandardViewModel svm = view_matrix.parse();
	look_direction = svm.look_at.normalize();
	up = svm.up.normalize();
}
Camera::camera_()
{
}
void Camera::update(vec3 camera_pos)
{
	eye = camera_pos;
	view_matrix = mat4::look_at(eye, eye + look_direction, up);
}
void Camera::set_aspect_ratio(float ratio)
{
	this->aspect_ratio = ratio;
}
void Camera::revalidate_projection_matrix()
{
	this->projection_matrix = mat4::perspective(fov, aspect_ratio, dNear, dFar);
}
inline void Camera::look_direction_adjust(float x, float y)
{
	vec3 right = look_direction.cross(up).normalize();
	view_matrix *= mat4::rotate(right, y * 0.001f) * mat4::rotate(vec3(0, 1, 0), x * 0.001f);
	revalidate_all();
}

inline void Camera::set_looking_at(vec3 look)
{
	look_direction = look - eye;
	look_direction.normalize();
}