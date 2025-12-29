#ifndef TOSU_OPTIONS
#define TOSU_OPTIONS


#include <raylib.h>


#define OK 0
#define FAIL 1
#define UNIMPLEMENTED 2
#define ERROR 3
#define ERROR_CHART_NOT_LOADED 4
#define ERROR_GAMESPACE_NOT_LOADED 5

#define DEFAULT_WINDOW_SIZE (Vector2){960, 720} // 4:3 ratio

#define KEY_AMOUNT 26


/*
 * I got confused on how to use KeycodeBindings so this is now here.
 * Keyboard - int that stores info about key presses in each bit
 * Keycode - char that specifies a letter, 0 - 25
 * KeycodeBindings - list of Keycodes with Keycodes as indices.
 */

typedef int KeycodeBindings[KEY_AMOUNT];
typedef int* KeycodeBindingsW;

typedef struct Options {
    KeycodeBindings bindings;
} Options;

typedef struct WindowManager {
    int width, height;
    float x, y;
} WindowManager;


void DefaultBindings(KeycodeBindingsW bindings);

int GetKeyboardInput(void);
int GetKeyboardInputRelease(void);
int KeycodeToKeyboard(char code, KeycodeBindings bindings);
char KeyboardToKeycode(int key, KeycodeBindings bindings);
char KeycodeBind(char code, KeycodeBindings bindings);

void DefaultOptions(void);

void UpdateWindowManager(void);
Vector2 ScreenScale(void);
Vector2 ScreenSize(void);


#endif
