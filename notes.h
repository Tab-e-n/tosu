#ifndef TOSU_NOTES
#define TOSU_NOTES

#include <raylib.h>
#include "options.h"


#define HIT_NULL       0b00000
#define HIT_PERFECT    0b00001
#define HIT_GOOD_EARLY 0b00010
#define HIT_GOOD_LATE  0b01010
#define HIT_OK_EARLY   0b00100
#define HIT_OK_LATE    0b01100
#define HIT_MISS       0b10000
#define HIT_PENALTY    0b11000

#define SCORE_POINTS_PENALTY -200
#define SCORE_POINTS_OK      100
#define SCORE_POINTS_GOOD    300
#define SCORE_POINTS_PERFECT 500

#define BASE_HIT_WINDOW 14
#define NOTE_SPAWN_WINDOW 120
#define INV_NOTE_SPAWN_WINDOW (1.0/NOTE_SPAWN_WINDOW)
#define NOTE_DESPAWN_WINDOW 120
#define INV_NOTE_DESPAWN_WINDOW (1.0/NOTE_DESPAWN_WINDOW)

#define NOTE_LIMIT 128


typedef struct Note {
    bool active,
         hold,
         mine,
         being_held;
    int time, time_end;
    char key, color,
         score, score_end;
} Note;

typedef struct GameSpace {
    char difficulty, speed;
    int time, time_end, score, offset,
        current_note,
        current_code;
    double accuracy_sum, accuracy_mult;
    Color colors[4]; // 0 is the color of mines, 1-3 color of notes
    Note notes[NOTE_LIMIT];
} GameSpace;


int HitWindow(int difficulty, int hit);
int HitScorePoints(char score);
double HitAccuracy(char score);

bool NotePressed(Note note, int input, KeycodeBindings bindings);
char NoteHitScore(Note note, int hit_time, char difficulty, int offset);
char NoteHoldScore(Note note, int release, char difficulty);

GameSpace GameInit(void);
void GameAddAccuracy(GameSpace* game, double acc);
double GameGetAccuracy(GameSpace* game);
bool GameAddNote(GameSpace* game, Note note);
bool GameMakeNote(GameSpace* game, int time, char key, char color, bool mine);
bool GameMakeHoldNote(GameSpace* game, int start, int end, char key, char color, bool mine);
void GameProcessNotes(GameSpace* game);
bool GameShouldEnd(GameSpace* game);

void DebugDrawNote(Note note, int time);
void DebugDrawNoteOutline(Note note, Color color);
void DebugDrawGame(GameSpace* game);

#endif
