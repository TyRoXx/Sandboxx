#include "adventure_state_view.h"
#include "base/adventure_state.h"
#include <stdlib.h>
#include <assert.h>


static GameStateView *AdventureStateView_create(GameState *state)
{
	AdventureState * const adv_state = (AdventureState *)state;
	AdventureStateView * const view = malloc(sizeof(*view));
	assert(adv_state);

	if (!view)
	{
		return 0;
	}

	view->state = adv_state;
	return (GameStateView *)view;
}

static void AdventureStateView_destroy(GameStateView *view)
{
	free(view);
}

static void AdventureStateView_draw(GameStateView *view)
{
	/*TODO*/
}

static void AdventureStateView_handle_event(GameStateView *view, SDL_Event const *event)
{

}

GameStateViewType const AdventureStateViewType =
{
	AdventureStateView_create,
	AdventureStateView_destroy,
	AdventureStateView_draw,
	AdventureStateView_handle_event
};
