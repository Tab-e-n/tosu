#include <raylib.h>
#include "options.h"
#include "notes.h"

int HitWindow(int difficulty, int hit)
{
    return (BASE_HIT_WINDOW - difficulty) * hit;
}

int HitScorePoints(char score)
{
    switch(score)
    {
        case HIT_OK_LATE:
        case HIT_OK_EARLY:
            return SCORE_POINTS_OK;
        case HIT_GOOD_LATE:
        case HIT_GOOD_EARLY:
            return SCORE_POINTS_GOOD;
        case HIT_PERFECT:
            return SCORE_POINTS_PERFECT;
        case HIT_PENALTY:
            return SCORE_POINTS_PENALTY;
        default:
            return 0;
    }
}

double HitAccuracy(char score)
{
    switch(score)
    {
        case HIT_OK_LATE:
        case HIT_OK_EARLY:
            return 0.4;
        case HIT_GOOD_LATE:
        case HIT_GOOD_EARLY:
            return 0.8;
        case HIT_PERFECT:
            return 1.0;
        default:
            return 0.0;
    }
}

bool NotePressed(Note note, int input, KeycodeBindings bindings)
{
    return input & KeycodeToKeyboard(note.key, bindings);
}

char NoteHitScore(Note note, int hit_time, char difficulty, int offset)
{
    int inacc = (hit_time + offset) - note.time;

    if(note.mine)
    {
        int hit_window = HitWindow(difficulty, 2);

        if(inacc >= -hit_window && inacc <= hit_window)
        {
            return HIT_PENALTY;
        }
        return HIT_MISS;
    }

    int hit_perfect = HitWindow(difficulty, 1);
    int hit_good = HitWindow(difficulty, 2);
    int hit_ok = HitWindow(difficulty, 3);

    if(inacc <= hit_perfect && inacc >= -hit_perfect)
    {
        return HIT_PERFECT;
    }
    if(inacc > 0)
    {
        if(inacc <= hit_good)
        {
            return HIT_GOOD_LATE;
        }
        else if(inacc <= hit_ok)
        {
            return HIT_OK_LATE;
        }
    }
    if(inacc < 0)
    {
        if(inacc >= -hit_good)
        {
            return HIT_GOOD_EARLY;
        }
        else if(inacc >= -hit_ok)
        {
            return HIT_OK_EARLY;
        }
    }
    return HIT_MISS;
}

char NoteHoldScore(Note note, int release, char difficulty)
{
    if(release >= note.time_end)
    {
        return HIT_PERFECT;
    }
    int hit_perfect = HitWindow(difficulty, 1);
    int hit_good = HitWindow(difficulty, 2);
    int hit_ok = HitWindow(difficulty, 3);

    int inacc = note.time_end - release;
    if(inacc <= hit_perfect)
    {
        return HIT_PERFECT;
    }
    else if(inacc <= hit_good)
    {
        return HIT_GOOD_EARLY;
    }
    else if(inacc <= hit_ok)
    {
        return HIT_OK_EARLY;
    }
    return HIT_MISS;
}

GameSpace GameInit(void)
{
    GameSpace game = (GameSpace){0};
    //game.accuracy_mult = 1;
    game.colors[0] = RED;
    game.colors[1] = YELLOW;
    game.colors[2] = GREEN;
    game.colors[3] = SKYBLUE;
    return game;
}

void GameAddAccuracy(GameSpace* game, double acc)
{
    game->accuracy_sum += acc;
    if(game->accuracy_mult == 0.0)
    {
	game->accuracy_mult = 1.0;
    }
    else
    {
	game->accuracy_mult /= game->accuracy_mult + 1;
    }
    TraceLog(LOG_INFO, "Acc: %g %g", game->accuracy_sum, game->accuracy_mult);
}

double GameGetAccuracy(GameSpace* game)
{
    return game->accuracy_sum * game->accuracy_mult;
}

bool GameAddNote(GameSpace* game, Note note)
{
    int loops = 0;
    while(loops < NOTE_LIMIT && game->notes[game->current_note].active)
    {
        game->current_note++;
        if(game->current_note >= NOTE_LIMIT)
        {
            game->current_note = 0;
        }
        loops++;
    }
    if(loops >= NOTE_LIMIT)
    {
        TraceLog(LOG_INFO, "GAME: couldn't add note");
        return 1;
    }
    game->notes[game->current_note] = note;
    TraceLog(LOG_INFO, "GAME: Added note");
    return 0;
}

bool GameMakeNote(GameSpace* game, int time, char key, char color, bool mine)
{
    Note note = (Note){0};
    note.active = true;
    note.time = time;
    note.key = key;
    note.color = color;
    note.mine = mine;
    return GameAddNote(game, note);
}

bool GameMakeHoldNote(GameSpace* game, int start, int end, char key, char color, bool mine)
{
    Note note = (Note){0};
    note.active = true;
    note.hold = true;
    note.time = start;
    note.time_end = end;
    note.key = key;
    note.color = color;
    note.mine = mine;
    return GameAddNote(game, note);
}

void GameProcessNotes(GameSpace* game, Options* options)
{
    int input = GetKeyboardInput();
    int release = GetKeyboardInputRelease();
    for(int i = 0; i < NOTE_LIMIT; i++)
    {
        Note note = game->notes[i];
        if(!note.active)
        {
            continue;
        }
        if(note.being_held)
        {
            if(note.time_end + HitWindow(game->difficulty, 3) < game->time)
            {
                TraceLog(LOG_INFO, "SCORE: Held till end");
                game->score += HitScorePoints(HIT_PERFECT);
		GameAddAccuracy(game, HitAccuracy(HIT_PERFECT));
                note.active = false;
            }
            else if(NotePressed(note, release, options->bindings))
            {
                char score = NoteHoldScore(note, game->time, game->difficulty);
                if(score)
                {
                    TraceLog(LOG_INFO, "SCORE: Ended hold early");
                    game->score += HitScorePoints(score);
		    GameAddAccuracy(game, HitAccuracy(score));
                }
                else
                {
                    TraceLog(LOG_INFO, "MISS: ended hold too early");
                }
                note.active = false;
            }
        }
        else
        {
            if(note.time + HitWindow(game->difficulty, 3) < game->time)
            {
                TraceLog(LOG_INFO, "MISS: Didn't hit key");
                note.active = false;
            }
            if(NotePressed(note, input, options->bindings))
            {
                char score = NoteHitScore(note, game->time, game->difficulty, game->offset);
                if(score)
                {
                    TraceLog(LOG_INFO, "SCORE: Hit note");
                    game->score += HitScorePoints(score);
		    GameAddAccuracy(game, HitAccuracy(score));

                    if(note.hold)
                    {
                        note.being_held = true;
                    }
                    else
                    {
                        note.active = false;
                    }
                }
                else
                {
                    TraceLog(LOG_INFO, "Not yet in hit window");
                }
            }
        }
        game->notes[i] = note;

    }
    game->time++;
}

void DebugDrawNote(Note note, int time)
{
    if(!note.active)
    {
        return;
    }
    Vector2 position = (Vector2){32, 96};
    if(note.key < 10)
    {
        position.x += note.key * 40;
    }
    else if(note.key < 19)
    {
        position.x += (note.key - 10) * 40 + 10;
        position.y += 40;
    }
    else
    {
        position.x += (note.key - 19) * 40 + 30;
        position.y += 80;
    }
    char text[KEY_AMOUNT] = {
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
        'Z', 'X', 'C', 'V', 'B', 'N', 'M',
    };
    if(note.hold)
    {
        if(note.time < time)
        {
            DrawRectangle(position.x, position.y, 32, 32, GRAY);
            DrawRectangle(position.x, position.y, (note.time_end - time) * 0.25, 32, BLUE);
        }
        else
        {
            DrawRectangle(position.x, position.y, 32, 32, GRAY);
            DrawRectangle(position.x, position.y, (note.time - time) * 0.25, 32, SKYBLUE);
        }
        DrawText(TextFormat("%c", text[note.key]), position.x, position.y, 24, BLACK);
    }
    else if(note.mine)
    {
        DrawRectangle(position.x, position.y, 32, 32, BLACK);
        DrawRectangle(position.x, position.y, (note.time - time) * 0.25, 32, RED);
        DrawText(TextFormat("%c", text[note.key]), position.x, position.y, 24, WHITE);
    }
    else
    {
        DrawRectangle(position.x, position.y, 32, 32, LIGHTGRAY);
        DrawRectangle(position.x, position.y, (note.time - time) * 0.25, 32, YELLOW);
        DrawText(TextFormat("%c", text[note.key]), position.x, position.y, 24, BLACK);
    }
}

void DebugDrawNoteOutline(Note note, Color color)
{
    Vector2 position = (Vector2){28, 92};
    if(note.key < 10)
    {
        position.x += note.key * 40;
    }
    else if(note.key < 19)
    {
        position.x += (note.key - 10) * 40 + 10;
        position.y += 40;
    }
    else
    {
        position.x += (note.key - 19) * 40 + 30;
        position.y += 80;
    }
    DrawRectangle(position.x, position.y, 40, 40, color);
    DrawRectangle(position.x + 2, position.y + 2, 36, 36, RAYWHITE);
    DrawText(TextFormat("%i", note.color), position.x - 16, position.y, 24, BLACK);
}

void DebugDrawGame(GameSpace* game)
{
    for(int i = 0; i < NOTE_LIMIT; i++)
    {
        Note note = game->notes[i];
        DebugDrawNote(note, game->time);
    }
    DrawText("GAMEPLAY", 32, 32, 24, BLACK);
    DrawText(TextFormat("%i", game->score), 32, 64, 24, BLACK);
    DrawText(TextFormat("%i", game->time), 160, 64, 24, BLACK);
    DrawText(TextFormat("%.3f", GameGetAccuracy(game)), 288, 64, 24, BLACK);
}
