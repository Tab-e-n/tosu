#include <raylib.h>
#include "options.h"


void BindingsDefault(KeycodeBindingsW bindings)
{
    for(int i = 0; i < KEY_AMOUNT; i++)
    {
        bindings[i] = i;
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
    if(code < 0 || code >= KEY_AMOUNT)
    {
        return 0;
    }
    return 1 << bindings[code];
}

char KeyboardToKeycode(int key, KeycodeBindings bindings)
{
    int code = 0;
    while(1 << bindings[code] != key)
    {
        code++;
        if(code >= KEY_AMOUNT)
        {
            return 0;
        }
    }
    return code;
}

char KeycodeBind(char code, KeycodeBindings bindings)
{
    if(code < 0 || code >= KEY_AMOUNT)
    {
        return 0;
    }
    return bindings[code];
}

Options options = (Options){0};

void OptionsDefault(void)
{
    options = (Options){0};
    BindingsDefault(options.bindings);
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

float ScreenScaleDominant(void)
{
    const Vector2 SCREEN_SCALE = ScreenScale();
    return SCREEN_SCALE.x > SCREEN_SCALE.y ? SCREEN_SCALE.y : SCREEN_SCALE.x;
}

Vector2 ScreenSize(void)
{
    Vector2 size = (Vector2){wscale.width, wscale.height};
    return size;
}

bool InputTiming(char* timer, bool reset)
{
    if(reset)
    {
        *timer = MENU_MOVE_TIMER * 2;
        return false;
    }
    if(*timer == MENU_MOVE_TIMER * 2)
    {
        --*timer;
        return true;
    }
    if(*timer <= 0)
    {
        *timer = MENU_MOVE_TIMER;
        return true;
    }
    --*timer;
    return false;
}

MenuList InitMenuList(int min, int max)
{
    MenuList menu = (MenuList){0};
    menu.chosen = false;
    menu.min = min;
    menu.max = max;
    menu.current = min;
    menu.move_timer = MENU_MOVE_TIMER;
    return menu;
}

bool MenuListSet(MenuList* menu, int pos)
{
    if(menu->current == pos) return FAIL;
    menu->current = pos;
    return MenuListMove(menu, 0);
}

bool MenuListMove(MenuList* menu, int move)
{
    menu->current += move;
    if(menu->current < menu->min)
    {
        menu->current = menu->min;
        return FAIL;
    }
    if(menu->current > menu->max)
    {
        menu->current = menu->max;
        return FAIL;
    }
    return OK;
}

bool MenuListMoveTimed(MenuList* menu, int move)
{
    if(InputTiming(&menu->move_timer, move == 0))
    {
        return MenuListMove(menu, move);
    }
    return OK;
}

void MenuListChoose(MenuList* menu)
{
    menu->chosen = true;
}

void MenuListUnchoose(MenuList* menu)
{
    menu->chosen = false;
}

bool MenuListHasChosen(MenuList* menu)
{
    return menu->chosen;
}

int MenuListCurrent(MenuList* menu)
{
    return menu->current;
}

void LoadNewDirectory(FilePathList* files, const char* dir)
{
    UnloadLoadedDirectory(files);
    *files = LoadDirectoryFiles(dir);
}

void LoadNewDirectoryEx(FilePathList* files, const char* dir, const char* filter)
{
    UnloadLoadedDirectory(files);
    *files = LoadDirectoryFilesEx(dir, filter, false);
}

void UnloadLoadedDirectory(FilePathList* files)
{
    if(files->count != 0) UnloadDirectoryFiles(*files);
}
