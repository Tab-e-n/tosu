#include <raylib.h>
#include "options.h"
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
    //sprites.mine_hit_circle = LoadTexture("sprites/note_mine_hit_circle.png");

    sprites.score_100 = LoadTexture("sprites/hit_score_100.png");
    sprites.score_300 = LoadTexture("sprites/hit_score_300.png");
    sprites.score_500 = LoadTexture("sprites/hit_score_500.png");
    sprites.score_miss = LoadTexture("sprites/hit_miss.png");
    sprites.score_penalty = LoadTexture("sprites/hit_penalty_200.png");

    for(int i = 0; i < KEY_AMOUNT; i++)
    {
        sprites.keys[i] = LoadTexture(TextFormat("sprites/note_letter_%i.png", i));
    }

    return sprites;
}

Texture AddSprite(Texture source, Texture add)
{
    if(IsTextureValid(source))
    {
        return source;
    }
    return add;
}

void AddGameSprites(GameplaySprites* source, GameplaySprites add)
{
    source->normal_base = AddSprite(source->normal_base, add.normal_base);
    source->normal_outline = AddSprite(source->normal_outline, add.normal_outline);
    source->normal_hit_circle = AddSprite(source->normal_hit_circle, add.normal_hit_circle);

    source->hold_base = AddSprite(source->hold_base, add.hold_base);
    source->hold_outline = AddSprite(source->hold_outline, add.hold_outline);
    source->hold_hit_circle = AddSprite(source->hold_hit_circle, add.hold_hit_circle);

    source->mine_base = AddSprite(source->mine_base, add.mine_base);
    source->mine_outline = AddSprite(source->mine_outline, add.mine_outline);

    source->score_100 = AddSprite(source->score_100, add.score_100);
    source->score_300 = AddSprite(source->score_300, add.score_300);
    source->score_500 = AddSprite(source->score_500, add.score_500);
    source->score_miss = AddSprite(source->score_miss, add.score_miss);
    source->score_penalty = AddSprite(source->score_penalty, add.score_penalty);

    for(int i = 0; i < KEY_AMOUNT; i++)
    {
        source->keys[i] = AddSprite(source->keys[i], add.keys[i]);
    }
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

int NoteAlpha(Note note, int game_time)
{
    int anim_time = note.time - game_time;
    int alpha = 0;
    //alpha = alpha < 0 ? -alpha : alpha;
    if(anim_time >= 0)
    {
        alpha = anim_time;
        alpha -= ALPHA_HOLD_FRAMES;
        alpha = alpha < 0 ? 0 : alpha;
        alpha *= 255;
        alpha /= NOTE_SPAWN_WINDOW - ALPHA_HOLD_FRAMES;
        alpha = 255 - alpha;
    }
    if(note.hold)
    {
        anim_time = note.time_end - game_time;
    }
    if(anim_time < 0)
    {
        alpha = anim_time;
        alpha = -alpha;
        alpha -= ALPHA_HOLD_FRAMES;
        alpha = alpha < 0 ? 0 : alpha;
        alpha = alpha > ALPHA_DISAPPEAR_FRAMES ? ALPHA_DISAPPEAR_FRAMES : alpha;
        alpha *= 255;
        alpha /= ALPHA_DISAPPEAR_FRAMES;
        alpha = 255 - alpha;
    }
    return alpha;
}

void DrawNote(Note note, GameSpace* game, Options* options, GameplaySprites sprites)
{
    // TODO: Clean up this code a tad
    const Vector2 SCREEN_SIZE = ScreenSize();
    const Vector2 SCREEN_SCALE = ScreenScale();
    float scale = 1.0;
    scale = (SCREEN_SCALE.x > SCREEN_SCALE.y ? SCREEN_SCALE.y : SCREEN_SCALE.x) * 0.25;
    const int SPRITE_SIZE = 320;
    const int SPACING = scale * (SPRITE_SIZE + 32);
    const int ROW_2_OFFSET = scale * (SPRITE_SIZE >> 2);
    const int ROW_3_OFFSET = ROW_2_OFFSET * 3;
    const Vector2 START_POS = (Vector2)
    {
        (SCREEN_SIZE.x - SPACING * 10) * 0.5,
        (SCREEN_SIZE.y - SPACING * 3) * 0.5
    };

    if(!note.active)
    {
        return;
    }
    Vector2 position = START_POS;
    if(note.key < FIRST_KEY_ROW)
    {
        position.x += note.key * SPACING;
    }
    else if(note.key < SECOND_KEY_ROW)
    {
        position.x += (note.key - FIRST_KEY_ROW) * SPACING + ROW_2_OFFSET;
        position.y += SPACING;
    }
    else
    {
        position.x += (note.key - SECOND_KEY_ROW) * SPACING + ROW_3_OFFSET;
        position.y += 2 * SPACING;
    }

    Color base_color = game->colors[note.color];
    Color other_color = (note.mine ? MINE_COLOR : OUTLINE_COLOR);

    int alpha = NoteAlpha(note, game->time);
    base_color.a = alpha;
    other_color.a = alpha;

    int anim_time = note.time - game->time;
    if(note.hold)
    {
        DrawTextureEx(sprites.hold_base, position, 0.0, scale, base_color);
        DrawTextureEx(sprites.hold_outline, position, 0.0, scale, other_color);
        DrawTextureEx(sprites.keys[note.key], position, 0.0, scale, other_color);
    }
    else if(note.mine)
    {
        DrawTextureEx(sprites.mine_base, position, 0.0, scale, other_color);
        DrawTextureEx(sprites.mine_outline, position, 0.0, scale, base_color);
        DrawTextureEx(sprites.keys[note.key], position, 0.0, scale, base_color);
    }
    else
    {
        DrawTextureEx(sprites.normal_base, position, 0.0, scale, base_color);
        DrawTextureEx(sprites.normal_outline, position, 0.0, scale, other_color);
        DrawTextureEx(sprites.keys[note.key], position, 0.0, scale, other_color);

        int hit_time = anim_time < 0 ? 0 : anim_time;
        float hit_circle_scale = scale + hit_time * scale / NOTE_SPAWN_WINDOW;
        Vector2 hit_circle_position = position;
        float offset = hit_time * scale * SPRITE_SIZE / NOTE_SPAWN_WINDOW * 0.5;
        hit_circle_position.x -= offset;
        hit_circle_position.y -= offset;
        DrawTextureEx(sprites.normal_hit_circle, hit_circle_position, 0.0, hit_circle_scale, other_color);
    }

    if(note.score != HIT_NULL)
    {
        int score_time = anim_time > 0 ? 0 : anim_time;
        Vector2 score_position = position;
        score_position.y += SPRITE_SIZE;
        Texture used_texture;
        switch(note.score)
        {
            case(HIT_PERFECT):
                used_texture = sprites.score_500;
                break;
            case(HIT_GOOD_EARLY):
            case(HIT_GOOD_LATE):
                used_texture = sprites.score_300;
                break;
            case(HIT_OK_EARLY):
            case(HIT_OK_LATE):
                used_texture = sprites.score_100;
                break;
            case(HIT_MISS):
                used_texture = sprites.score_miss;
                break;
            case(HIT_PENALTY):
                used_texture = sprites.score_penalty;
                break;
        }
        DrawTextureEx(used_texture, position, 0.0, scale, WHITE);
    }
}

void GameDrawNotes(GameSpace* game, Options* options, GameplaySprites sprites)
{
    for(int i = 0; i < NOTE_LIMIT; i++)
    {
        Note note = game->notes[i];
        DrawNote(note, game, options, sprites);
    }
}
