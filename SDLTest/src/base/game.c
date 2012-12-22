#include "game.h"
#include "adventure_state.h"


int Game_init(Game *g)
{
	g->state = 0;
	return Game_enter_state(g, &AdventureStateDef);
}

void Game_free(Game *g)
{
	g->state->definition->destroy(g->state);
}

int Game_update(Game *g, unsigned delta)
{
	g->state->definition->update(g->state, delta);
	return 1;
}

int Game_enter_state(Game *g, GameStateDefinition const *state)
{
	GameState * const new_state = state->create(g);
	if (!new_state)
	{
		return 0;
	}
	new_state->definition = state;
	if (g->state)
	{
		g->state->definition->destroy(g->state);
	}
	g->state = new_state;
	return 1;
}
