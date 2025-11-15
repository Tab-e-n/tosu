#ifndef TOSU_VISUALS
#define TOSU_VISUALS

#include <raylib.h>
#include "notes.h"


#define DEFAULT_WINDOW_SIZE (Vector2){960, 720} // 4:3 ratio

// TODO: Non raylib default colors
#define MINE_COLOR BLACK
#define OUTLINE_COLOR WHITE


typedef struct GameplaySprites {
    Texture normal_base, normal_outline, normal_hit_circle;
    Texture hold_base, hold_outline, hold_hit_circle;
    Texture mine_base, mine_outline;
    Texture keys[KEY_AMOUNT];
    Texture score_100, score_300, score_500, score_miss, score_penalty;
} GameplaySprites;

typedef struct RankSprites {
    Texture unranked;
    // TODO: Decide what ranks the player can get, their requirements, where the code for ranks will be stored
} RankSprites;


// Load sprites from the current_directory/sprites/
GameplaySprites LoadGameSprites(void);
// Have a separate place where you combine them
Texture AddSprite(Texture source, Texture add);
void AddGameSprites(GameplaySprites* source, GameplaySprites add);
// Once you are done using them (Player finished level), Discard the separate place and unload level sprites
// Level and skin get unloaded when the game is closed
void UnloadGameplaySprites(GameplaySprites* sprites);

// Drawing in the editor may need to be done using a dummy GameSpace
void DrawNote(Note note, GameSpace* game, KeycodeBindings bindings, GameplaySprites sprites);
void GameDrawNotes(GameSpace* game, KeycodeBindings bindings, GameplaySprites sprites);

void DrawScore(GameSpace* game);
void DrawTimebar(GameSpace* game);

RankSprites LoadRankSprites(void);
void UnloadRankSprites(RankSprites sprites);

//void DrawRank(..., RankSprites sprites);


#endif

