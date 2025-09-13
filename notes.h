#ifndef TOSU_NOTES
#define TOSU_NOTES

#include <raylib.h>


#define KEY_AMOUNT 26

#define HIT_MISS       0b0000
#define HIT_PERFECT    0b0001
#define HIT_GOOD_LATE  0b1010
#define HIT_GOOD_EARLY 0b0010
#define HIT_OK_LATE    0b1100
#define HIT_OK_EARLY   0b0100
#define HIT_PENALTY    0b10000

#define BASE_HIT_WINDOW 14
#define NOTE_SPAWN_WINDOW 120

#define NOTE_LIMIT 128


typedef struct KeycodeBindings {
	int codes[KEY_AMOUNT];
} KeycodeBindings;

typedef struct Note {
	bool active,
	     hold,
	     mine,
	     being_held;
	int time, time_end;
	char key;
} Note;

typedef struct GameSpace {
	int time, score,
	    current_note,
	    current_code;
	Note notes[NOTE_LIMIT];
	char difficulty, speed;
} GameSpace;


KeycodeBindings DefaultBindings();

int GetKeyboardInput();
int GetKeyboardInputRelease();
int KeycodeToKeyboard(char code, KeycodeBindings bindings);
char KeyboardToKeycode(int key, KeycodeBindings bindings);

int HitWindow(int difficulty, int hit);
int HitScorePoints(char score);

bool NotePressed(Note note, int input, KeycodeBindings bindings);
char NoteHitScore(Note note, int hit_time, char difficulty);
char NoteHoldScore(Note note, int release, char difficulty);

bool GameAddNote(GameSpace* game, Note note);
bool GameMakeNote(GameSpace* game, int time, char key, bool mine);
bool GameMakeHoldNote(GameSpace* game, int start, int end, char key, bool mine);
void GameProcessNotes(GameSpace* game, KeycodeBindings bindings);

void DebugDrawNote(Note note, int time);
void DebugDrawGame(GameSpace* game);

#endif
