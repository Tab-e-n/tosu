#include <raylib.h>
#include "notes.h"
#include "chart.h"


#define SCREEN_SIZE (Vector2){640, 480}

typedef enum GameScene {LOGO, MENU, GAME, EDITOR} GameScene;


int main(void)
{
	InitWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, "TOSU");

	SetTargetFPS(120);

	GameScene scene = EDITOR;

	GameSpace game = (GameSpace){0};
	KeycodeBindings bindings = DefaultBindings();
	char key = 0;

	EditorChart editor = (EditorChart){0};

	while(!WindowShouldClose())
	{
		switch(scene)
		{
			case GAME:
				if(game.time % 300 == 0)
				{
					if(GameMakeHoldNote(&game, game.time + 120, game.time + 180, key, false))
					{
						//TraceLog(LOG_WARNING, "No more space for notes");
					}
					key++;
					if(key >= KEY_AMOUNT) key = 0;
				}
				GameProcessNotes(&game, bindings);
				break;
			case EDITOR:
				//TraceLog(LOG_INFO, "Get Keyboard Input");
				int input = GetKeyboardInput();
				//TraceLog(LOG_INFO, "%i", input);
				if(input)
				{
					//TraceLog(LOG_INFO, "new note");
					Note note = (Note){0};
					note.time = editor.current_time;
					note.key = KeyboardToKeycode(input, bindings);
					EditorAddNote(&editor, note);
				}
				if(IsKeyPressed(KEY_BACKSPACE))
				{
					EditorRemoveNote(&editor);
				}
				break;
		}

		BeginDrawing();

			ClearBackground(RAYWHITE);

		switch(scene)
		{
			case GAME:
				DebugDrawNotes(&game);
				break;
			case EDITOR:
				DebugDrawEditor(&editor);
				break;
		}

			DrawFPS(16, 16);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
