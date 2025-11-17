#include <raylib.h>
#include "options.h"


void DefaultBindings(KeycodeBindingsW bindings)
{
    for(int i = 0; i < KEY_AMOUNT; i++)
    {
        bindings[i] = 1 << i;
    }
}

const int KEYBOARD[KEY_AMOUNT] = {
    KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,
    KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L,
    KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M,
};

int GetKeyboardInput()
{
    int input = 0;

    for(int i = 0; i < KEY_AMOUNT; i++)
    {
        input += IsKeyPressed(KEYBOARD[i]) << i;
    }

    return input;
}

int GetKeyboardInputRelease()
{
    int input = 0;

    for(int i = 0; i < KEY_AMOUNT; i++)
    {
        input += IsKeyReleased(KEYBOARD[i]) << i;
    }

    return input;
}

int KeycodeToKeyboard(char code, KeycodeBindings bindings)
{
    return bindings[code];
}

char KeyboardToKeycode(int key, KeycodeBindings bindings)
{
    int code = 0;
    while(bindings[code] != key)
    {
        code++;
        if(code >= KEY_AMOUNT)
        {
            return 0;
        }
    }
    return code;
}

Options DefaultOptions(void)
{
    Options options = (Options){0};
    DefaultBindings(options.bindings);
    return options;
}

WindowManager wscale = (WindowManager){0};

void UpdateWindowManager(void)
{
    if(wscale.width != GetScreenWidth() || wscale.height != GetScreenHeight())
    {
        wscale.width = GetScreenWidth();
        wscale.height = GetScreenHeight();
        wscale.x = GetScreenWidth() / (float)DEFAULT_WINDOW_SIZE.x;
        wscale.y = GetScreenHeight() / (float)DEFAULT_WINDOW_SIZE.y;
    }
}

Vector2 ScreenScale(void)
{
    Vector2 scale = (Vector2){wscale.x, wscale.y};
    return scale;
}

Vector2 ScreenSize(void)
{
    Vector2 size = (Vector2){wscale.width, wscale.height};
    return size;
}
