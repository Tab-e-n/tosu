#ifndef TOSU_OPTIONS
#define TOSU_OPTIONS


#include <raylib.h>


#define DEFAULT_WINDOW_SIZE (Vector2){960, 720} // 4:3 ratio

#define KEY_AMOUNT 26


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

int GetKeyboardInput();
int GetKeyboardInputRelease();
int KeycodeToKeyboard(char code, KeycodeBindings bindings);
char KeyboardToKeycode(int key, KeycodeBindings bindings);

Options DefaultOptions();

void UpdateWindowManager(void);
Vector2 ScreenScale(void);
Vector2 ScreenSize(void);


#endif
