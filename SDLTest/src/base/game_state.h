#ifndef GAME_STATE_H
#define GAME_STATE_H


struct GameState;
struct Game;


typedef struct GameStateDefinition
{
	struct GameState *(*create)(struct Game *);
	void (*destroy)(struct GameState *);
	void (*update)(struct GameState *, unsigned delta);
}
GameStateDefinition;


typedef struct GameState
{
	GameStateDefinition const *definition;
}
GameState;



#endif
