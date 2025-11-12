#include <raylib.h>
#include "notes.h"
#include "visuals.h"


GameplaySprites LoadGameSprites(void)
{
    GameplaySprites sprites = (GameplaySprites){0};
    sprites.normal_base = LoadTexture("sprites/note_normal_base.png");
    sprites.normal_outline = LoadTexture("sprites/note_normal_outline.png");
    sprites.normal_hit_circle = LoadTexture("sprites/note_normal_hit_circle.png");

    sprites.hold_base = LoadTexture("sprites/note_hold_base.png");
    sprites.hold_outline = LoadTexture("sprites/note_hold_outline.png");
    sprites.hold_hit_circle = LoadTexture("sprites/note_hold_hit_circle.png");

    sprites.mine_base = LoadTexture("sprites/note_mine_base.png");
    sprites.mine_outline = LoadTexture("sprites/note_mine_outline.png");
    sprites.mine_hit_circle = LoadTexture("sprites/note_mine_hit_circle.png");

    sprites.score_100 = LoadTexture("sprites/hit_score_100.png");
    sprites.score_300 = LoadTexture("sprites/hit_score_300.png");
    sprites.score_500 = LoadTexture("sprites/hit_score_500.png");
    sprites.score_miss = LoadTexture("sprites/hit_miss.png");
    sprites.score_penalty = LoadTexture("sprites/hit_penalty_200.png");

    for(int i = 0; i < KEY_AMOUNT; i++)
    {
        sprites.keys[i] = LoadTexture(TextFormat("sprites/note_letter_%c", 'a' + i));
    }

    return sprites;
}

GameplaySprites AddGameSprites(GameplaySprites sprites)
{
    // IsTextureValid(texture);
}

void UnloadGameplaySprites(GameplaySprites* sprites)
{
    UnloadTexture(sprites->normal_base);
    UnloadTexture(sprites->normal_outline);
    UnloadTexture(sprites->normal_hit_circle);

    UnloadTexture(sprites->hold_base);
    UnloadTexture(sprites->hold_outline);
    UnloadTexture(sprites->hold_hit_circle);

    UnloadTexture(sprites->mine_base);
    UnloadTexture(sprites->mine_outline);
    UnloadTexture(sprites->mine_hit_circle);

    UnloadTexture(sprites->score_100);
    UnloadTexture(sprites->score_300);
    UnloadTexture(sprites->score_500);
    UnloadTexture(sprites->score_miss);
    UnloadTexture(sprites->score_penalty);

    for(int i = 0; i < KEY_AMOUNT; i++)
    {
        UnloadTexture(sprites->keys[i]);
    }
}

void DrawNormalNote()
{
    // DrawTexture(texture, x, y, tint);
}
