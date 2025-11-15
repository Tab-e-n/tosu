#ifndef TOSU_NOTES
#define TOSU_NOTES

#include <raylib.h>
#include "options.h"


#define HIT_MISS       0b0000
#define HIT_PERFECT    0b0001
#define HIT_GOOD_LATE  0b1010
#define HIT_GOOD_EARLY 0b0010
#define HIT_OK_LATE    0b1100
#define HIT_OK_EARLY   0b0100
#define HIT_PENALTY    0b10000

#define SCORE_POINTS_PENALTY -200
#define SCORE_POINTS_OK      100
#define SCORE_POINTS_GOOD    300
#define SCORE_POINTS_PERFECT 500

#define BASE_HIT_WINDOW 14
#define NOTE_SPAWN_WINDOW 120

#define NOTE_LIMIT 128


typedef struct Note {
    bool active,
         hold,
         mine,
         being_held;
    int time, time_end;
    char key, color;
    // TODO: Store info on what hit score the player got so you can draw particles
} Note;

typedef struct GameSpace {
    char difficulty, speed;
    int time, score, offset,
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
void GameProcessNotes(GameSpace* game, Options* options);

void DebugDrawNote(Note note, int time);
void DebugDrawNoteOutline(Note note, Color color);
void DebugDrawGame(GameSpace* game);

#endif
