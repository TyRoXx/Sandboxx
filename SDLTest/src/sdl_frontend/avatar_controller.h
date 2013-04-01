#ifndef AVATAR_CONTROLLER_H
#define AVATAR_CONTROLLER_H


#include "base/entity.h"


struct Entity;


typedef struct AvatarController
{
	struct Entity *avatar;
	int is_direction_key_down[4];
}
AvatarController;


Bool AvatarController_init(AvatarController *a, struct Entity *avatar);
void AvatarController_free(AvatarController *a);
void AvatarController_handle_input(AvatarController *a, Direction dir, int is_down);
void AvatarController_update(AvatarController *a);


#endif
