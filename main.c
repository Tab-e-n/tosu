#include <raylib.h>
#include "options.h"
#include "notes.h"
#include "chart.h"
#include "visuals.h"


typedef enum GameScene {LOGO, MENU, GAME, EDITOR} GameScene;
typedef enum MainMenuOptions {MAIN_PLAY, MAIN_EDIT, MAIN_OPTIONS, MAIN_EXIT} MainMenuOptions;
typedef enum EditorMode {MAIN, INSERT_NORMAL, INSERT_HOLD, INSERT_MINE, EDIT_NOTE} EditorMode;


int main(void)
{
    ChangeDirectory(GetApplicationDirectory());
    ChangeDirectory("resources");

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(DEFAULT_WINDOW_SIZE.x, DEFAULT_WINDOW_SIZE.y, "TOSU");

    SetTargetFPS(120);
    SetExitKey(KEY_NULL);

    GameScene scene = MENU;
    bool window_running = true;
    FilePathList files = (FilePathList){0};

    extern Options options;
    DefaultOptions();
    /*
    for(int i = 0; i < KEY_AMOUNT; i++)
    {
        options.bindings[i] = KEY_AMOUNT - i - 1;
    }
    */
    char key = 0;

    MenuList main_menu = InitMenuList(0, 3);

    Chart* chart = (Chart*)0;
    GameSpace game = GameInit();

    EditorChart editor = (EditorChart){0};
    EditorMode mode = MAIN;
    Note hold_note = (Note){0};
    char current_color = 1;
    bool edit_duration = false;

    GameplaySprites game_sprites = LoadGameSprites();

    while(!WindowShouldClose() && window_running)
    {
        UpdateWindowManager();
        switch(scene)
        {
        case MENU:
            if(MenuListHasChosen(&main_menu) && MenuListCurrent(&main_menu) == MAIN_OPTIONS)
            {
                if(IsKeyPressed(KEY_ESCAPE))
                {
                    MenuListUnchoose(&main_menu);
                }
            }
            else if(MenuListHasChosen(&main_menu))
            {
                if(IsKeyPressed(KEY_ESCAPE))
                {
                    MenuListUnchoose(&main_menu);
                }
            }
            else
            {
                if(IsKeyDown(KEY_UP))
                {
                    MenuListMoveTimed(&main_menu, -1);
                }
                if(IsKeyDown(KEY_DOWN))
                {
                    MenuListMoveTimed(&main_menu, 1);
                }
                if(!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
                {
                    MenuListMoveTimed(&main_menu, 0);
                }
                if(IsKeyPressed(KEY_ESCAPE))
                {
                    if(MenuListSet(&main_menu, MAIN_EXIT))
                    {
                        MenuListChoose(&main_menu);
                    }
                }
                if(IsKeyPressed(KEY_ENTER))
                {
                    MenuListChoose(&main_menu);
                }
                if(MenuListHasChosen(&main_menu))
                {
                    switch(MenuListCurrent(&main_menu))
                    {
                    case MAIN_PLAY:
                        LoadNewDirectory(&files, "charts");
                        break;
                    case MAIN_EDIT:
                        LoadNewDirectory(&files, "charts");
                        break;
                    case MAIN_OPTIONS:
                        break;
                    case MAIN_EXIT:
                        window_running = false;
                        break;
                    }
                }
            }
            BeginDrawing();
                if(MenuListHasChosen(&main_menu))
                {
                    switch(MenuListCurrent(&main_menu))
                    {
                    case MAIN_PLAY:
                        ClearBackground((Color){155, 222, 155, 255});
                        break;
                    case MAIN_EDIT:
                        ClearBackground((Color){222, 222, 155, 255});
                        break;
                    case MAIN_OPTIONS:
                        ClearBackground((Color){155, 155, 222, 255});
                        break;
                    }
                }
                else
                {
                    ClearBackground((Color){222, 155, 222, 255});

                    const char* text[] = {"PLAY", "EDIT", "OPTIONS", "EXIT"};
                    for(int i = 0; i <= 3; i++)
                    {
                        Color color = BLACK;
                        if(MenuListCurrent(&main_menu) == i)
                        {
                            color = GRAY;
                        }
                        DrawText(text[i], 32, 32 + 32 * i, 24, color);
                    }
                }

                DrawFPS(16, 16);
            EndDrawing();

            break;

        case GAME:
            while(ChartShouldReadNext(chart, &game))
            {
                ChartReadNext(chart, &game);
            }
            GameProcessNotes(&game);
            if(IsKeyPressed(KEY_ENTER))
            {
                free(chart);
                scene = EDITOR;
                game = GameInit();
            }

            BeginDrawing();
                ClearBackground((Color){22, 15, 22, 255});
                // DebugDrawGame(&game);
                GameDrawNotes(&game, game_sprites);
                DrawFPS(16, 16);
            EndDrawing();

            break;

        case EDITOR:
	    // TraceLog(LOG_INFO, "k:\t ");
            int input = GetKeyboardInput();
            if(mode == INSERT_NORMAL || mode == INSERT_MINE)
            {
                if(input)
                {
                    Note note = (Note){0};
                    note.active = true;
                    note.time = editor.current_time;
                    note.key = KeyboardToKeycode(input, options.bindings);
                    if(mode == INSERT_MINE)
                    {
                        note.mine = true;
			note.color = 0;
                    }
		    else
		    {
			note.color = current_color;
		    }
                    EditorAddNote(&editor, note);
                }
            }
            if(mode == INSERT_HOLD)
            {
                if(input && !hold_note.active)
                {
                    hold_note = (Note){0};
                    hold_note.active = true;
                    hold_note.time = editor.current_time;
                    hold_note.key = KeyboardToKeycode(input, options.bindings);
                    hold_note.color = current_color;
                    hold_note.hold = true;
                }
                else if(input && hold_note.active)
                {
                    hold_note.time_end = editor.current_time;
                    EditorAddNote(&editor, hold_note);
                    hold_note.active = false;
                }
            }
	    if(mode == EDIT_NOTE)
	    {
                if(editor.current != (void*)0 && input)
		{
                    editor.current->note.key = KeyboardToKeycode(input, options.bindings);
		}
		if(IsKeyPressed(KEY_TAB))
		{
		    edit_duration = !edit_duration;
		    if(edit_duration)
		    {
			TraceLog(LOG_INFO, "tab:\t Edit Hold Duration");
		    }
		    else
		    {
			TraceLog(LOG_INFO, "tab:\t Edit Note Time");
		    }
		}
	    }
            if(mode == MAIN)
            {
                if(IsKeyPressed(KEY_ENTER))
                {
                    chart = EditorToChart(&editor);
                    scene = GAME;
                    game = GameInit();
                    game.time = -60;
                }
                if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
                {
		    TraceLog(LOG_INFO, "ctrl+s:\t Save Chart");
                    Chart* temp_chart = EditorToChart(&editor);
                    SaveChart(temp_chart, "test.chart");
                    free(temp_chart);
                }
                if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_L))
                {
		    TraceLog(LOG_INFO, "ctrl+l:\t Load Chart");
                    ChartLoadResult result = LoadChart("test.chart");
                    if(result.success)
                    {
                        EditorClearNotes(&editor);
                        ChartToEditor(result.chart, &editor);
                    }
                    free(result.chart);
                }
                if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O))
                {
                    TraceLog(LOG_INFO, "ctrl+o:\t Clear Chart");
                    EditorClearNotes(&editor);
                }
                if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P))
                {
                    TraceLog(LOG_INFO, "ctrl+p:\t Print Editor Notes");
                    PrintEditor(&editor);
                }
                if(IsKeyPressed(KEY_N))
                {
                    TraceLog(LOG_INFO, "n:\t Mode INSERT NORMAL");
                    mode = INSERT_NORMAL;
                }
                if(IsKeyPressed(KEY_H))
                {
                    TraceLog(LOG_INFO, "h:\t Mode INSERT HOLD");
                    mode = INSERT_HOLD;
                }
                if(IsKeyPressed(KEY_M))
                {
                    TraceLog(LOG_INFO, "m:\t Mode INSERT MINE");
                    mode = INSERT_MINE;
                }
		if(IsKeyPressed(KEY_E))
		{
		    TraceLog(LOG_INFO, "e:\t Mode EDIT NOTE");
		    mode = EDIT_NOTE;
		}
            }
            // GENERIC EDITOR
            if(IsKeyPressed(KEY_BACKSPACE))
            {
                EditorRemoveNote(&editor);
		/*
		if(mode == EDIT_NOTE)
		{
		    TraceLog(LOG_INFO, "Exited EDIT NOTE to MAIN");
		    mode = MAIN;
		}
		*/
            }
	    // current_color = 0 is reserved for mines
            const int COLOR_KEYS[] = {KEY_ONE, KEY_TWO, KEY_THREE};
            for(int i = 0; i < 3; i++)
            {
                if(IsKeyPressed(COLOR_KEYS[i]))
                {
                    current_color = i + 1;
                    TraceLog(LOG_INFO, "%i:\t Color %i", current_color, current_color);
                    if(mode == EDIT_NOTE) EditorColorCurrentNote(&editor, current_color);
                }
            }

            bool left_pressed = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_COMMA);
            bool right_pressed = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_PERIOD);

            if(IsKeyDown(KEY_LEFT_CONTROL) && mode != EDIT_NOTE)
            {
                if(left_pressed)
                {
		    TraceLog(LOG_INFO, "ctrl+left:\t First Note / Start");
                    EditorMoveToStart(&editor);
                }
                if(right_pressed)
                {
		    TraceLog(LOG_INFO, "ctrl+right:\t Last Note / End");
                    EditorMoveToEnd(&editor);
                }
            }
            else
            {
                char speed = IsKeyDown(KEY_LEFT_SHIFT) ? 1 : 3;
                if(IsKeyDown(KEY_LEFT_ALT))
                {
                    speed *= 4;
                }
                if(!IsKeyDown(KEY_PERIOD) && !IsKeyDown(KEY_COMMA))
                {
                    speed *= MOVE_DELAY_FRAMES;
                }

		if(mode == EDIT_NOTE && edit_duration)
		{
		    if(editor.current != (void*)0 && editor.current->note.hold)
		    {
			if(left_pressed && InputTiming(&editor.move_timer, false))
			{
			    editor.current->note.time_end -= speed;
			    if(editor.current->note.time_end < 0)
			    {
				editor.current->note.time_end = 0;
			    }
			}
			if(right_pressed && InputTiming(&editor.move_timer, false))
			{
			    editor.current->note.time_end += speed;
			}
		    }
		}
		else if(mode == EDIT_NOTE)
		{
		    if(left_pressed && InputTiming(&editor.move_timer, false))
		    {
			EditorMoveCurrentNote(&editor, -speed);
		    }
		    if(right_pressed && InputTiming(&editor.move_timer, false))
		    {
			EditorMoveCurrentNote(&editor, speed);
		    }
		}
		else
		{
		    if(left_pressed)
		    {
			EditorMoveTimed(&editor, -speed);
		    }
		    if(right_pressed)
		    {
			EditorMoveTimed(&editor, speed);
		    }
		}
            }
            if(IsKeyDown(KEY_UP) && InputTiming(&editor.move_timer, false))
            {
                EditorMoveToNext(&editor);
            }
            if(IsKeyDown(KEY_DOWN) && InputTiming(&editor.move_timer, false))
            {
                EditorMoveToPrevious(&editor);
            }
            if(!left_pressed && !right_pressed && !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
            {
                InputTiming(&editor.move_timer, true);
            }
            if(IsKeyPressed(KEY_ESCAPE))
            {
		TraceLog(LOG_INFO, "esc:\t Mode MAIN");
                mode = MAIN;
                hold_note.active = false;
            }

            BeginDrawing();
                ClearBackground((Color){248, 224, 255, 255});
                if(hold_note.active)
                {
                    DebugDrawNoteOutline(hold_note, BLUE);
                    DrawText(TextFormat("(%i)", hold_note.time), 96, 64, 24, BLUE);
                }
		if(edit_duration && editor.current != (void*)0)
		{
                    DrawText(TextFormat("(%i)", editor.current->note.time_end), 96, 64, 24, BLUE);
		}
                DebugDrawEditor(&editor);
                if(mode == INSERT_NORMAL)
                {
                    DrawScreenInline(8.0, YELLOW);
                    DrawText("INSERT NORMAL", 144, 32, 24, BLACK);
                }
                if(mode == INSERT_HOLD)
                {
                    DrawScreenInline(8.0, BLUE);
                    DrawText("INSERT HOLD", 144, 32, 24, BLACK);
                }
                if(mode == INSERT_MINE)
                {
                    DrawScreenInline(8.0, RED);
                    DrawText("INSERT MINE", 144, 32, 24, BLACK);
                }
		if(mode == EDIT_NOTE)
		{
                    DrawScreenInline(8.0, PINK);
                    DrawText("EDIT NOTE", 144, 32, 24, BLACK);
		}
                DrawFPS(16, 16);
            EndDrawing();
            break;
        }
    }

    UnloadGameplaySprites(&game_sprites);

    CloseWindow();

    return 0;
}
