#include <raylib.h>
#include "options.h"
#include "notes.h"
#include "chart.h"
#include "visuals.h"


typedef enum GameScene {LOGO, MENU, GAME, EDITOR} GameScene;
typedef enum MainMenuOptions {MAIN_PLAY, MAIN_EDIT, MAIN_OPTIONS, MAIN_EXIT} MainMenuOptions;


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
    OptionsDefault();
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
    EditorDefault(&editor);

    ChartFilename filename = (ChartFilename){0};
    ChartFilenameFill(&filename, "test");
    char entering_filename = 0;
    char entering_filename_timer = 0;

    GameplaySprites game_sprites = LoadGameSprites();

    while(!WindowShouldClose() && window_running)
    {
        UpdateWindowManager();
        switch(scene)
        {
        case MENU:
            if(MenuListHasChosen(&main_menu) && (MenuListCurrent(&main_menu) == MAIN_OPTIONS))
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
                        scene = EDITOR;
                        //LoadNewDirectory(&files, "charts");
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
            if(entering_filename)
            {
                if(IsKeyDown(KEY_BACKSPACE) && InputTiming(&entering_filename_timer, false))
                {
                    ChartFilenameRemoveChar(&filename);
                }
                if(!IsKeyDown(KEY_BACKSPACE))
                {
                    InputTiming(&entering_filename_timer, true);
                }

                char c = GetCharPressed();
                if(c)
                {
                    ChartFilenameAddChar(&filename, c);
                }
                if(IsKeyPressed(KEY_ENTER))
                {
                    ChartFilenameSuffix(&filename);
                    if(entering_filename == 1)
                    {
                        ChartLoadResult result = LoadChart(filename.str);
                        if(result.success)
                        {
                            EditorClearNotes(&editor);
                            ChartToEditor(result.chart, &editor);
                        }
                        free(result.chart);
                    }
                    if(entering_filename == 2)
                    {
                        Chart* temp_chart = EditorToChart(&editor);
                        SaveChart(temp_chart, filename.str);
                        free(temp_chart);
                    }
                    ChartFilenameRemoveSuffix(&filename);
                    entering_filename = 0;
                }
                if(IsKeyPressed(KEY_ESCAPE))
                {
                    ChartFilenameRemoveSuffix(&filename);
                    entering_filename = 0;
                }
            }
            else
            {
                EditorProcessCode process_code = EditorProcess(&editor);

                switch(process_code)
                {
                    case EDITOR_EXIT_PLAYTEST:
                        chart = EditorToChart(&editor);
                        scene = GAME;
                        game = GameInit();
                        game.time = -60;
                        break;
                    case EDITOR_EXIT_MENU:
                        EditorDefault(&editor);
                        scene = MENU;
                        break;
                    case EDITOR_LOAD_CHART:
                        entering_filename = 1;
                        break;
                    case EDITOR_SAVE_CHART:
                        entering_filename = 2;
                        break;
                }
            }

            BeginDrawing();
                ClearBackground((Color){248, 224, 255, 255});
                DebugDrawEditor(&editor);
                if(editor.mode == EM_INSERT_NORMAL)
                {
                    DrawScreenInline(8.0, YELLOW);
                    DrawText("INSERT NORMAL", 144, 32, 24, BLACK);
                }
                if(editor.mode == EM_INSERT_HOLD)
                {
                    DrawScreenInline(8.0, BLUE);
                    DrawText("INSERT HOLD", 144, 32, 24, BLACK);
                }
                if(editor.mode == EM_INSERT_MINE)
                {
                    DrawScreenInline(8.0, RED);
                    DrawText("INSERT MINE", 144, 32, 24, BLACK);
                }
                if(editor.mode == EM_EDIT_NOTE)
                {
                    DrawScreenInline(8.0, PINK);
                    DrawText("EDIT NOTE", 144, 32, 24, BLACK);
                }
                if(editor.mode == EM_EXIT)
                {
                    DrawScreenInline(8.0, BLACK);
                    DrawText("PRESS ENTER TO EXIT", 144, 32, 24, BLACK);
                }
                if(entering_filename)
                {
                    if(entering_filename == 1)
                    {
                        DrawText("LOADING:", 32, 224, 24, BLACK);
                    }
                    if(entering_filename == 2)
                    {
                        DrawText("SAVING:", 32, 224, 24, BLACK);
                    }
                    DrawText(filename.str, 32, 256, 24, BLACK);
                }
                DrawFPS(16, 16);
            EndDrawing();
            break;
        }
    }

    UnloadLoadedDirectory(&files);
    UnloadGameplaySprites(&game_sprites);

    CloseWindow();

    return 0;
}
