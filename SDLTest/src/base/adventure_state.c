#include "adventure_state.h"
#include "world_text_file.h"
#include "game.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static TileKind const TileKinds[] =
{
	{0, 1},
	{1, 1},
	{2, 1},
	{3, 1},
	{4, 0},
	{5, 1},
	{6, 1},
	{7, 1},
	{8, 0},
	{9, 0},
};

static Bool load_world(char const *file_name, World *world)
{
	Bool result;
	FILE * const file = fopen(file_name, "r");
	if (!file)
	{
		fprintf(stderr, "Cannot open world file %s\n", file_name);
		return False;
	}

	result = load_world_from_text(world,
								  TileKinds,
								  sizeof(TileKinds) / sizeof(*TileKinds),
								  file,
								  stderr);
	fclose(file);
	return result;
}

static GameState *AdventureState_create(Game *game)
{
	AdventureState * const adv_state = malloc(sizeof(*adv_state));
	if (!adv_state)
	{
		return NULL;
	}

	(void)game;

	{
		char const * const world_file_name = "data/world.txt";
		World * const world = &adv_state->world;

		if (load_world(world_file_name, world))
		{
			/*if there is an entity, choose the first one as the avatar*/
			adv_state->avatar = adv_state->world.entities;
			return (GameState *)adv_state;
		}

		return NULL;
	}
}

static void AdventureState_destroy(GameState *state)
{
	AdventureState * const adv_state = (AdventureState *)state;
	World_free(&adv_state->world);
	free(state);
}

static void AdventureState_update(GameState *state, unsigned delta)
{
	AdventureState * const adv_state = (AdventureState *)state;
	World_update(&adv_state->world, delta);
}

GameStateDefinition const AdventureStateDef =
{
	AdventureState_create,
	AdventureState_destroy,
	AdventureState_update
};
