#include <raylib.h>
#include "notes.h"
#include "chart.h"


#define SCREEN_SIZE (Vector2){640, 480}

typedef enum GameScene {LOGO, MENU, GAME, EDITOR} GameScene;


int main(void)
{
	InitWindow(SCREEN_SIZE.x, SCREEN_SIZE.y, "TOSU");

	SetTargetFPS(120);

	GameScene scene = MENU;

	GameSpace game = (GameSpace){0};
	KeycodeBindings bindings = DefaultBindings();
	char key = 0;

	while(!WindowShouldClose())
	{
		switch(scene)
		{
			case MENU:
				if(game.time % 300 == 0)
				{
					if(GameMakeHoldNote(&game, game.time + 120, game.time + 180, key, false))
					{
						TraceLog(LOG_WARNING, "No more space for notes");
					}
					key++;
					if(key >= KEY_AMOUNT) key = 0;
				}
				GameProcessNotes(&game, bindings);
				break;
		}

		BeginDrawing();

			ClearBackground(RAYWHITE);

			DebugDrawNotes(&game);

			DrawFPS(16, 16);

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
