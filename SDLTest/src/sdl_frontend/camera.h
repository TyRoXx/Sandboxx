#ifndef CAMERA_H
#define CAMERA_H


#include "base/vector2f.h"
#include "base/entity.h"


typedef struct Camera
{
	Vector2f position;
}
Camera;


int Camera_init(Camera *cam);
void Camera_free(Camera *cam);
void Camera_focus_on(Camera *cam, struct Entity const *target);


float get_move_offset(Direction move_dir, float progress, Direction dir);
float get_entity_offset(Entity const *e, Direction dir);


#endif
