#include <raylib.h>
#include "notes.h"
#include "chart.h"


#define SCREEN_SIZE (Vector2){640, 480}

typedef enum GameScene {LOGO, MENU, GAME, EDITOR} GameScene;
typedef enum EditorMode {MAIN, INSERT_NORMAL, INSERT_HOLD, INSERT_MINE, EDIT_NOTE} EditorMode;


int main(void)
{
    InitWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, "TOSU");

    SetTargetFPS(120);
    SetExitKey(KEY_NULL);

    GameScene scene = EDITOR;

    Chart* chart = (Chart*)0;
    GameSpace game = GameInit();
    KeycodeBindings bindings = DefaultBindings();
    char key = 0;

    EditorChart editor = (EditorChart){0};
    EditorMode mode = MAIN;
    Note hold_note = (Note){0};
    char current_color = 1;
    bool edit_duration = false;

    while(!WindowShouldClose())
    {
        switch(scene)
        {
        case GAME:
            while(ChartShouldReadNext(chart, &game))
            {
                ChartReadNext(chart, &game);
            }
            GameProcessNotes(&game, bindings);
            if(IsKeyPressed(KEY_ENTER))
            {
                free(chart);
                scene = EDITOR;
                game = GameInit();
            }
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
                    note.key = KeyboardToKeycode(input, bindings);
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
                    hold_note.key = KeyboardToKeycode(input, bindings);
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
                    editor.current->note.key = KeyboardToKeycode(input, bindings);
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
            if(IsKeyPressed(KEY_ONE))
            {
		TraceLog(LOG_INFO, "1:\t Color 1");
                current_color = 1;
		if(mode == EDIT_NOTE) EditorColorCurrentNote(&editor, current_color);
            }
            if(IsKeyPressed(KEY_TWO))
            {
		TraceLog(LOG_INFO, "2:\t Color 2");
                current_color = 2;
		if(mode == EDIT_NOTE) EditorColorCurrentNote(&editor, current_color);
            }
            if(IsKeyPressed(KEY_THREE))
            {
		TraceLog(LOG_INFO, "3:\t Color 3");
                current_color = 3;
		if(mode == EDIT_NOTE) EditorColorCurrentNote(&editor, current_color);
            }
            if(IsKeyDown(KEY_LEFT_CONTROL) && mode != EDIT_NOTE)
            {
                if(IsKeyPressed(KEY_LEFT))
                {
		    TraceLog(LOG_INFO, "ctrl+left:\t First Note / Start");
                    EditorMoveToStart(&editor);
                }
                if(IsKeyPressed(KEY_RIGHT))
                {
		    TraceLog(LOG_INFO, "ctrl+right:\t Last Note / End");
                    EditorMoveToEnd(&editor);
                }
            }
            else
            {
                char speed = MOVE_DELAY_FRAMES;
                if(IsKeyDown(KEY_LEFT_SHIFT))
                {
                    speed *= 0.33334;
                }
                if(IsKeyDown(KEY_LEFT_ALT))
                {
                    speed *= 4;
                }
		if(mode == EDIT_NOTE && edit_duration)
		{
		    if(editor.current != (void*)0 && editor.current->note.hold)
		    {
			if(IsKeyDown(KEY_LEFT) && EditorTiming(&editor, false))
			{
			    editor.current->note.time_end -= speed;
			    if(editor.current->note.time_end < 0)
			    {
				editor.current->note.time_end = 0;
			    }
			}
			if(IsKeyDown(KEY_RIGHT) && EditorTiming(&editor, false))
			{
			    editor.current->note.time_end += speed;
			}
		    }
		}
		else if(mode == EDIT_NOTE)
		{
		    if(IsKeyDown(KEY_LEFT) && EditorTiming(&editor, false))
		    {
			EditorMoveCurrentNote(&editor, -speed);
		    }
		    if(IsKeyDown(KEY_RIGHT) && EditorTiming(&editor, false))
		    {
			EditorMoveCurrentNote(&editor, speed);
		    }
		}
		else
		{
		    if(IsKeyDown(KEY_LEFT))
		    {
			EditorMoveTimed(&editor, -speed);
		    }
		    if(IsKeyDown(KEY_RIGHT))
		    {
			EditorMoveTimed(&editor, speed);
		    }
		}
            }
            if(IsKeyDown(KEY_UP) && EditorTiming(&editor, false))
            {
                EditorMoveToNext(&editor);
            }
            if(IsKeyDown(KEY_DOWN) && EditorTiming(&editor, false))
            {
                EditorMoveToPrevious(&editor);
            }
            if(!IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
            {
                EditorTiming(&editor, true);
            }
            if(IsKeyPressed(KEY_ESCAPE))
            {
		TraceLog(LOG_INFO, "esc:\t Mode MAIN");
                mode = MAIN;
            }
            break;
        }

        BeginDrawing();


        Color bg;
        switch(scene)
        {
            case GAME:
                bg = (Color){232, 255, 232, 255};
                ClearBackground(bg);
                DebugDrawGame(&game);
                break;
            case EDITOR:
                bg = (Color){248, 224, 255, 255};
                ClearBackground(bg);
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
                    DrawText("INSERT NORMAL", 144, 32, 24, BLACK);
                }
                if(mode == INSERT_HOLD)
                {
                    DrawText("INSERT HOLD", 144, 32, 24, BLACK);
                }
                if(mode == INSERT_MINE)
                {
                    DrawText("INSERT MINE", 144, 32, 24, BLACK);
                }
		if(mode == EDIT_NOTE)
		{
                    DrawText("EDIT NOTE", 144, 32, 24, BLACK);
		}
                break;
        }

            DrawFPS(16, 16);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
