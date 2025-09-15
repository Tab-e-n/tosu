#include <raylib.h>
#include "notes.h"
#include "chart.h"


#define SCREEN_SIZE (Vector2){640, 480}

typedef enum GameScene {LOGO, MENU, GAME, EDITOR} GameScene;
typedef enum EditorMode {MAIN, INSERT_NORMAL, INSERT_HOLD, INSERT_MINE} EditorMode;


int main(void)
{
	InitWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, "TOSU");

	SetTargetFPS(120);
	SetExitKey(KEY_NULL);

	GameScene scene = EDITOR;

	Chart* chart = (Chart*)0;
	GameSpace game = (GameSpace){0};
	KeycodeBindings bindings = DefaultBindings();
	char key = 0;

	EditorChart editor = (EditorChart){0};
	EditorMode mode = MAIN;

	while(!WindowShouldClose())
	{
		switch(scene)
		{
		case GAME:
			/*
			if(game.time % 300 == 0)
			{
				if(GameMakeHoldNote(&game, game.time + 120, game.time + 180, key, false))
				{
					//TraceLog(LOG_WARNING, "No more space for notes");
				}
				key++;
				if(key >= KEY_AMOUNT) key = 0;
			}
			*/
			while(ChartShouldReadNext(chart, &game))
			{
				ChartReadNext(chart, &game);
			}
			GameProcessNotes(&game, bindings);
			if(IsKeyPressed(KEY_ENTER))
			{
				free(chart);
				scene = EDITOR;
				game = (GameSpace){0};
			}
			break;
		case EDITOR:
			//TraceLog(LOG_INFO, "Get Keyboard Input");
			int input = GetKeyboardInput();
			//TraceLog(LOG_INFO, "%i", input);
			if(mode == INSERT_NORMAL || mode == INSERT_MINE)
			{
				if(input)
				{
					//TraceLog(LOG_INFO, "new note");
					Note note = (Note){0};
					note.active = true;
					note.time = editor.current_time;
					note.key = KeyboardToKeycode(input, bindings);
					if(mode == INSERT_MINE)
					{
						note.mine = true;
					}
					EditorAddNote(&editor, note);
				}
				if(IsKeyPressed(KEY_BACKSPACE))
				{
					EditorRemoveNote(&editor);
				}
			}
			if(mode == MAIN)
			{
				if(IsKeyPressed(KEY_ENTER))
				{
					chart = EditorToChart(&editor);
					TraceLog(LOG_INFO, "%i", chart->code_amount);
					scene = GAME;
					game = (GameSpace){0};
				}
				if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
				{
					Chart* temp_chart = EditorToChart(&editor);
					SaveChart(temp_chart, "test.chart");
					free(temp_chart);
				}
				if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_L))
				{
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
					EditorClearNotes(&editor);
				}
				if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P))
				{
					PrintEditor(&editor);
				}
				if(IsKeyPressed(KEY_N))
				{
					mode = INSERT_NORMAL;
					TraceLog(LOG_INFO, "INSERT NORMAL");
				}
				if(IsKeyPressed(KEY_H))
				{
					mode = INSERT_HOLD;
					TraceLog(LOG_INFO, "INSERT HOLD");
				}
				if(IsKeyPressed(KEY_M))
				{
					mode = INSERT_MINE;
					TraceLog(LOG_INFO, "INSERT MINE");
				}
			}
			// GENERIC EDITOR
			if(IsKeyDown(KEY_LEFT_CONTROL))
			{
				if(IsKeyPressed(KEY_LEFT))
				{
					EditorMoveToStart(&editor);
				}
				if(IsKeyPressed(KEY_RIGHT))
				{
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
				if(IsKeyDown(KEY_LEFT))
				{
					EditorMoveTimed(&editor, -speed);
				}
				if(IsKeyDown(KEY_RIGHT))
				{
					EditorMoveTimed(&editor, speed);
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
				mode = MAIN;
				TraceLog(LOG_INFO, "MAIN");
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
				break;
		}

			DrawFPS(16, 16);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
