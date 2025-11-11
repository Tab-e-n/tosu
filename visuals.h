#ifndef TOSU_VISUALS
#define TOSU_VISUALS

#include <raylib.h>
#include "notes.h"


// Use IsTextureValid() to check if texture is loaded


typedef struct GameplaySprites {
    Texture normal_base, normal_outline, normal_hit_circle;
    Texture hold_base, hold_outline, hold_hit_circle;
    Texture mine_base, mine_outline, mine_hit_circle;
    Texture keys[KEY_AMOUNT];
    Texture score_100, score_300, score_500, score_miss, score_penalty;
} GameplaySprites;

typedef struct RankSprites {
    Texture unranked;
    // TODO: Decide what ranks the player can get, their requirements, where the code for ranks will be stored
} RankSprites;


// Use these functions to load gameplay textures
GameplaySprites LoadGameLevelSprites(void);
GameplaySprites LoadGameSkinSprites(void);
GameplaySprites LoadGameDefaultSprites(void);
// Have a separate place where you combine them
GameplaySprites AddGameSprites(GameplaySprites source, GameplaySprites additional);
// Once you are done using them (Player finished level), Discard the separate place and unload level sprites
// Level and skin get unloaded when the game is closed
void UnloadGameplaySprites(GameplaySprites sprites);

// Drawing in the editor may need to be done using a dummy GameSpace
void DrawNormalNote(Note note, GameSpace* game, KeycodeBindings bindings, GameplaySprites sprites);
void DrawHoldNote(Note note, GameSpace* game, KeycodeBindings bindings, GameplaySprites sprites);
void DrawMineNote(Note note, GameSpace* game, KeycodeBindings bindings, GameplaySprites sprites);
void DrawNote(Note note, GameSpace* game, KeycodeBindings bindings, GameplaySprites sprites);
void GameDrawNotes(GameSpace* game, KeycodeBindings bindings, GameplaySprites sprites);

void DrawScore(GameSpace* game);
void DrawTimebar(GameSpace* game);

RankSprites LoadRankSprites(void);
void UnloadRankSprites(RankSprites sprites);

//void DrawRank(..., RankSprites sprites);


#endif

