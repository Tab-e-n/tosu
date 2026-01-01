#include <raylib.h>
#include "options.h"
#include "notes.h"
#include "chart.h"
#include "visuals.h"


typedef enum GameScene {LOGO, MENU, GAME, EDITOR} GameScene;
typedef enum MainMenuOptions {MAIN_PLAY, MAIN_EDIT, MAIN_OPTIONS, MAIN_EXIT} MainMenuOptions;


void ChangeDirToResources(void)
{
    ChangeDirectory(GetApplicationDirectory());
    ChangeDirectory("resources");
}

void LoadChartFolders(FilePathList* files, MenuList* menu)
{
    LoadNewDirectory(files, ".");
    *menu = InitMenuList(0, files->count - 1);
}

int main(void)
{
    ChangeDirToResources();
    MakeDirectory("charts");

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    InitWindow(DEFAULT_WINDOW_SIZE.x, DEFAULT_WINDOW_SIZE.y, "TOSU");

    SetTargetFPS(120);
    SetExitKey(KEY_NULL);

    GameScene scene = MENU;
    bool window_running = true;
    FilePathList files = (FilePathList){0};
    FilePathList chart_files = (FilePathList){0};

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
    MenuList file_select = InitMenuList(0, 0);
    MenuList chart_select  = InitMenuList(0, 0);

    Chart* chart = (Chart*)0;
    GameSpace game = GameInit();
    bool playtest = false;

    EditorChart editor = (EditorChart){0};
    EditorDefault(&editor);

    ChartFilename filename = (ChartFilename){0};
    ChartFilenameFill(&filename, "normal");
    ChartFilename dirname = (ChartFilename){0};
    ChartFilenameFill(&dirname, "unnamed");

    GameplaySprites game_sprites = LoadGameSprites();

    while(!WindowShouldClose() && window_running)
    {
        UpdateWindowManager();
        switch(scene)
        {
        case MENU:
            // Making new chart dir
            if(EnterChartFilenameActive(&dirname))
            {
                if(EnterChartFilename(&dirname, false))
                {
                    MakeDirectory(dirname.str);
                    ChangeDirectory(dirname.str);
                    UnloadLoadedDirectory(&files);
                    scene = EDITOR;
                    EnterChartFilenameDeactivate(&dirname);
                }
            }
            // Selecting chart
            else if(MenuListHasChosen(&file_select))
            {
                if(IsKeyDown(KEY_UP))
                {
                    MenuListMoveTimed(&chart_select, -1);
                }
                if(IsKeyDown(KEY_DOWN))
                {
                    MenuListMoveTimed(&chart_select, 1);
                }
                if(!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
                {
                    MenuListMoveTimed(&chart_select, 0);
                }
                if(IsKeyPressed(KEY_ENTER))
                {
                    ChartLoadResult result = LoadChart(chart_files.paths[MenuListCurrent(&chart_select)]);
                    if(result.success)
                    {
                        scene = GAME;
                        chart = result.chart;
                        game = GameInit();
                        game.time = -240;
                        playtest = false;
                    }
                    else
                    {
                        free(result.chart);
                    }
                }
                if(IsKeyPressed(KEY_ESCAPE))
                {
                    MenuListUnchoose(&file_select);
                }
            }
            // Options
            else if(MenuListHasChosen(&main_menu) && (MenuListCurrent(&main_menu) == MAIN_OPTIONS))
            {
                if(IsKeyPressed(KEY_ESCAPE))
                {
                    MenuListUnchoose(&main_menu);
                }
            }
            // Play & Edit
            else if(MenuListHasChosen(&main_menu))
            {
                if(IsKeyDown(KEY_UP))
                {
                    if(MenuListMoveTimed(&file_select, -1))
                    {
                        MenuListSet(&file_select, file_select.max);
                    }
                }
                if(IsKeyDown(KEY_DOWN))
                {
                    if(MenuListMoveTimed(&file_select, 1))
                    {
                        MenuListSet(&file_select, file_select.min);
                    }
                }
                if(!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
                {
                    MenuListMoveTimed(&file_select, 0);
                }
                if(IsKeyPressed(KEY_ESCAPE))
                {
                    MenuListUnchoose(&main_menu);
                    ChangeDirectory("..");
                }
                if(IsKeyPressed(KEY_ENTER))
                {
                    MenuListChoose(&file_select);
                }
                if(MenuListHasChosen(&file_select))
                {
                    if(MenuListCurrent(&main_menu) == MAIN_PLAY)
                    {
                        LoadNewDirectoryEx(&chart_files, files.paths[MenuListCurrent(&file_select)] + 2, ".chart");
                        chart_select = InitMenuList(0, chart_files.count - 1);
                    }
                    else if(MenuListCurrent(&file_select) != files.count)
                    {
                        ChangeDirectory(files.paths[MenuListCurrent(&file_select)] + 2);
                        UnloadLoadedDirectory(&files);
                        scene = EDITOR;
                        MenuListUnchoose(&file_select);
                    }
                    else
                    {
                        EnterChartFilenameActivate(&dirname, 1);
                        MenuListUnchoose(&file_select);
                    }
                }
            }
            // Main
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
                        ChangeDirectory("charts");
                        LoadChartFolders(&files, &file_select);
                        break;
                    case MAIN_EDIT:
                        ChangeDirectory("charts");
                        LoadChartFolders(&files, &file_select);
                        file_select.max += 1;
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
                if(EnterChartFilenameActive(&dirname))
                {
                    ClearBackground((Color){155, 222, 222, 255});
                    DrawText("NEW CHART:", 32, 32, 24, BLACK);
                    DrawText(dirname.str, 32, 64, 24, BLACK);
                }
                else if(MenuListHasChosen(&file_select))
                {
                    ClearBackground((Color){155, 222, 222, 255});
                    for(int i = 0; i < chart_files.count; i++)
                    {
                        Color color = BLACK;
                        if(MenuListCurrent(&chart_select) == i)
                        {
                            color = GRAY;
                        }
                        DrawText(chart_files.paths[i], 32, 32 + 32 * i, 24, color);
                    }
                }
                else if(MenuListHasChosen(&main_menu))
                {
                    Color color = BLACK;
                    int current = MenuListCurrent(&main_menu);
                    if(current == MAIN_PLAY || current == MAIN_EDIT)
                    {
                        ClearBackground((Color){155, 222, 222, 255});
                        for(int i = 0; i < files.count; i++)
                        {
                            color = BLACK;
                            if(MenuListCurrent(&file_select) == i)
                            {
                                color = GRAY;
                            }
                            DrawText(files.paths[i], 32, 32 + 32 * i, 24, color);
                        }
                    }
                    if(current == MAIN_EDIT)
                    {
                        color = BLACK;
                        if(MenuListCurrent(&file_select) == file_select.max)
                        {
                            color = GRAY;
                        }
                        DrawText("+ New chart", 32, 32 + 32 * file_select.max, 24, color);
                    }
                    if(current == MAIN_OPTIONS)
                    {
                        ClearBackground((Color){222, 222, 155, 255});
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

                DrawFPS(8, 8);
            EndDrawing();

            break;

        case GAME:
            bool exit_gameplay = false;
            while(ChartShouldReadNext(chart, &game))
            {
                ChartReadNext(chart, &game);
            }
            if(EndOfChart(chart, &game))
            {
                exit_gameplay = true;
            }
            GameProcessNotes(&game);
            if(IsKeyPressed(KEY_ESCAPE))
            {
                exit_gameplay = true;
            }

            if(exit_gameplay)
            {
                free(chart);
                game = GameInit();
                if(playtest)
                {
                    scene = EDITOR;
                }
                else
                {
                    scene = MENU;
                }
            }

            BeginDrawing();
                ClearBackground((Color){22, 15, 22, 255});
                // DebugDrawGame(&game);
                GameDrawNotes(&game, game_sprites);
                DrawFPS(8, 8);
            EndDrawing();

            break;

        case EDITOR:
            if(EnterChartFilenameActive(&filename))
            {
                char active_type = 0;
                if(active_type = EnterChartFilename(&filename, true))
                {
                    if(active_type == 1)
                    {
                        ChartLoadResult result = LoadChart(filename.str);
                        if(result.success)
                        {
                            EditorClearNotes(&editor);
                            ChartToEditor(result.chart, &editor);
                        }
                        free(result.chart);
                    }
                    if(active_type == 2)
                    {
                        Chart* temp_chart = EditorToChart(&editor);
                        SaveChart(temp_chart, filename.str);
                        free(temp_chart);
                    }
                    EnterChartFilenameDeactivate(&filename);
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
                        game.time = -120;
                        playtest = true;
                        break;
                    case EDITOR_EXIT_MENU:
                        EditorDefault(&editor);
                        scene = MENU;
                        ChangeDirectory("..");
                        LoadChartFolders(&files, &file_select);
                        file_select.max += 1;
                        break;
                    case EDITOR_LOAD_CHART:
                        EnterChartFilenameActivate(&filename, 1);
                        break;
                    case EDITOR_SAVE_CHART:
                        EnterChartFilenameActivate(&filename, 2);
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
                if(EnterChartFilenameActive(&filename) == 1)
                {
                    DrawText("LOADING:", 32, 224, 24, BLACK);
                    DrawText(filename.str, 32, 256, 24, BLACK);
                }
                if(EnterChartFilenameActive(&filename) == 2)
                {
                    DrawText("SAVING:", 32, 224, 24, BLACK);
                    DrawText(filename.str, 32, 256, 24, BLACK);
                }
                DrawFPS(8, 8);
            EndDrawing();
            break;
        }
    }

    UnloadLoadedDirectory(&files);
    UnloadGameplaySprites(&game_sprites);

    CloseWindow();

    return 0;
}
