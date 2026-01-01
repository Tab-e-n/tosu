#include <stdlib.h>
#include <raylib.h>
#include "options.h"
#include "notes.h"
#include "chart.h"

int NoteToInt(Note note)
{
    int code = 0;
    if(note.hold)
    {
        code += 0b10;
    }
    if(note.mine)
    {
        code += 0b100;
    }
    code += note.key << 3;
    code += note.color << 8;
    code += note.time << TIMECODE_SHIFT;
    //TraceLog(LOG_INFO, "%i, h%i m%i k%i t%i", code, note.hold, note.mine, note.key, note.time);
    return code;
}

Note IntToNote(int code)
{
    Note note = (Note){0};
    if(code & 0b10)
    {
        note.hold = true;
    }
    if(code & 0b100)
    {
        note.mine = true;
    }
    note.key = (code >> 3) & 0b11111;
    note.color = (code >> 8) & 0b11;
    note.time = code >> TIMECODE_SHIFT;
    note.active = true;
    //TraceLog(LOG_INFO, "%i, h%i m%i k%i t%i", code, note.hold, note.mine, note.key, note.time);
    return note;
}

int SizeOfChart(Chart* chart)
{
    if(chart != (void*)0)
    {
        return sizeof(Chart) + (sizeof(int) * chart->code_amount);
    }
    return sizeof(Chart);
}

bool ChartReadNext(Chart* chart, GameSpace* game)
{
    bool result = OK;
    if(chart == (void*)0)
    {
        return ERROR_CHART_NOT_LOADED;
    } 
    if(game == (void*)0)
    {
        return ERROR_GAMESPACE_NOT_LOADED;
    } 
    if(game->current_code < chart->code_amount)
    {
        int code = chart->codes[game->current_code];
        Note note = IntToNote(code);
        game->current_code++;
        if(note.hold)
        {
            note.time_end = chart->codes[game->current_code] >> TIMECODE_SHIFT;
            game->current_code++;
        }
        result = GameAddNote(game, note);
    }
    return result;
}

bool ChartShouldReadNext(Chart* chart, GameSpace* game)
{
    if(chart == (void*)0)
    {
        return false;
    } 
    if(game == (void*)0)
    {
        return false;
    } 
    if(game->current_code < chart->code_amount)
    {
        int delta = (chart->codes[game->current_code] >> TIMECODE_SHIFT) - game->time;
        return delta < NOTE_SPAWN_WINDOW;
    }
    return false;
}

bool EndOfChart(Chart* chart, GameSpace* game)
{
    if(chart == (void*)0)
    {
        return true;
    } 
    if(game->current_code >= chart->code_amount)
    {
        int delta = game->time - (chart->codes[chart->code_amount - 1] >> TIMECODE_SHIFT);
        return delta > NOTE_DESPAWN_WINDOW + CHART_END_DEADSPACE;
    }
    return false;
}

bool CopyChart(Chart* copy, Chart* chart)
{
    copy->difficulty = chart->difficulty;
    copy->speed = chart->speed;
    copy->code_amount = chart->code_amount;
    for(int i = 0; i < chart->code_amount; i++)
    {
        copy->codes[i] = chart->codes[i];
    }
    return OK;
}

ChartLoadResult LoadChart(const char* filename)
{
    int size;
    unsigned char* file = LoadFileData(filename, &size);
    ChartLoadResult result = (ChartLoadResult){0};
    
    if(file != (void*)0)
    {
        Chart* chart = (Chart*)malloc(size);
        CopyChart(chart, (Chart*)file);
        result.chart = chart;
        result.success = true;
    }
    else
    {
        result.chart = (void*)0;
        result.success = false;
    }

    UnloadFileData(file);

    return result;
}

bool SaveChart(Chart* chart, const char* filename)
{
    bool success = false;
    int data_size;
    int save_size;
    unsigned char* data_file = LoadFileData(filename, &data_size);
    unsigned char* save_file = (void*)0;

    if(data_file != (void*)0)
    {
        save_size = SizeOfChart(chart);
        if(data_size != save_size)
        {
            save_file = (unsigned char*)realloc(data_file, save_size);

            if(save_file != (void*)0)
            {
                Chart* save_chart = (Chart*)(save_file);
                CopyChart(save_chart, chart);
                //*save_chart = *chart;
            }
            else
            {
                save_size = data_size;
                save_file = data_file;
            }
        }
        else
        {
            save_size = data_size;
            save_file = data_file;

            Chart* save_chart = (Chart*)(save_file);
            CopyChart(save_chart, chart);
        }

        success = SaveFileData(filename, save_file, save_size);
        UnloadFileData(save_file);
    }
    else
    {
        save_size = SizeOfChart(chart);
        save_file = (unsigned char*)malloc(save_size);

        Chart* save_chart = (Chart*)(save_file);
        CopyChart(save_chart, chart);

        success = SaveFileData(filename, save_file, save_size);
        UnloadFileData(save_file);
    }

    return success;
}

void ChartFilenameClear(ChartFilename* filename)
{
    filename->current = 0;
    for(int i = 0; i < FILENAME_CHARACTERS_INTERNAL; i++)
    {
        filename->str[i] = '\0';
    }
}

bool ChartFilenameAddChar(ChartFilename* filename, char c)
{
    if(filename->current >= MAX_FILENAME_CHARACTERS)
    {
        return FAIL;
    }
    filename->str[filename->current] = c;
    filename->current++;
    return OK;
}

bool ChartFilenameRemoveChar(ChartFilename* filename)
{
    if(filename->current <= 0)
    {
        return FAIL;
    }
    filename->current--;
    filename->str[filename->current] = '\0';
    return OK;
}

void ChartFilenameSuffix(ChartFilename* filename)
{
    char suffix[] = ".chart";
    for(int i = 0; i < 6; i++)
    {
        filename->str[filename->current + i] = suffix[i];
    }
}

void ChartFilenameRemoveSuffix(ChartFilename* filename)
{
    for(int i = 0; i < 6; i++)
    {
        filename->str[filename->current + i] = '\0';
    }
}

bool ChartFilenameFill(ChartFilename* filename, const char* str)
{
    if(str == (void*)0)
    {
        return FAIL;
    }
    filename->current = 0;
    int i = 0;
    char c = 0;
    while((c = str[i]) != '\0')
    {
        filename->str[i++] = c;
        if(i >= MAX_FILENAME_CHARACTERS)
        {
            ChartFilenameClear(filename);
            return FAIL;
        }
    }
    filename->current = i;
    return OK;
}

void EnterChartFilenameActivate(ChartFilename* filename, int type)
{
    filename->active_type = type;
}

void EnterChartFilenameDeactivate(ChartFilename* filename)
{
    ChartFilenameRemoveSuffix(filename);
    filename->active_type = 0;
}

char EnterChartFilenameActive(ChartFilename* filename)
{
    return filename->active_type;
}

char EnterChartFilename(ChartFilename* filename, bool suffix)
{
    if(IsKeyDown(KEY_BACKSPACE) && InputTiming(&filename->timer, false))
    {
        ChartFilenameRemoveChar(filename);
    }
    if(!IsKeyDown(KEY_BACKSPACE))
    {
        InputTiming(&filename->timer, true);
    }

    char c = GetCharPressed();
    if(c)
    {
        ChartFilenameAddChar(filename, c);
    }
    if(IsKeyPressed(KEY_ENTER))
    {
        if(suffix) ChartFilenameSuffix(filename);
        return filename->active_type;
    }
    if(IsKeyPressed(KEY_ESCAPE))
    {
        EnterChartFilenameDeactivate(filename);
    }
    return 0;
}

bool EditorMoveToStart(EditorChart* editor)
{
    editor->current = editor->start;
    if(editor->current != (void*)0)
    {
        editor->current_time = editor->current->note.time;
        return OK;
    }
    editor->current_time = 0.0;
    return OK;
}

bool EditorMoveToEnd(EditorChart* editor)
{
    editor->current = editor->end;
    if(editor->current != (void*)0)
    {
        editor->current_time = editor->current->note.time;
        return OK;
    }
    return FAIL;
}

bool EditorMoveToNext(EditorChart* editor)
{
    if(editor->current == (void*)0 || editor->current->next == (void*)0)
    {
        return FAIL;
    }
    editor->current = editor->current->next;
    editor->current_time = editor->current->note.time;
    return OK;
    //return EditorMoveToTimecode(editor, editor->current->next->note.time);
}

bool EditorMoveToPrevious(EditorChart* editor)
{
    if(editor->current == (void*)0 || editor->current->previous == (void*)0)
    {
        return FAIL;
    }
    editor->current = editor->current->previous;
    editor->current_time = editor->current->note.time;
    return OK;
    //return EditorMoveToTimecode(editor, editor->current->previous->note.time);
}

bool EditorMove(EditorChart* editor, int time)
{
    editor->current_time += time;
    if(editor->current == (void*)0)
    {
        return OK;
    }
    EditorNote* current = editor->current;
    if(time > 0 && current->next != (void*)0)
    {
        while(current->next != (void*)0 && current->next->note.time <= editor->current_time)
        {
            current = current->next;
        }
    }
    if(time < 0 && current->previous != (void*)0)
    {
        while(current->previous != (void*)0 && current->previous->note.time >= editor->current_time)
        {
            current = current->previous;
        }
    }
    editor->current = current;
    return OK;
}

bool EditorMoveToTimecode(EditorChart* editor, int timecode)
{
    return EditorMove(editor, timecode - editor->current_time);
}

bool EditorMoveTimed(EditorChart* editor, int time)
{
    // EditorTiming is reset when time == 0, returns false when resetting
    char before = editor->move_timer;
    if(InputTiming(&editor->move_timer, time == 0))
    {
        TraceLog(LOG_INFO, "%i %i XXX", before, editor->move_timer);
        return EditorMove(editor, time);
    }
    TraceLog(LOG_INFO, "%i %i", before, editor->move_timer);
    return OK;
}

bool EditorAddNote(EditorChart* editor, Note note)
{
    //TraceLog(LOG_INFO, "Editor Add Note malloc");
    EditorNote* enote = malloc(sizeof(EditorNote));
    enote->next = (void*)0;
    enote->previous = (void*)0;
    //TraceLog(LOG_INFO, "Editor Add Note check");
    if(enote == (void*)0)
    {
        return ERROR;
    }
    enote->note = note;

    if(editor->current != (void*)0)
    {
        //TraceLog(LOG_INFO, "current exists");
        if(editor->current->note.time <= enote->note.time)
        {
            // Adding the note after current
            enote->previous = editor->current;
            enote->next = editor->current->next;

            editor->current->next = enote;
            if(enote->next != (void*)0)
            {
                enote->next->previous = enote;
            }
            else
            {
                editor->end = enote;
            }
        }
        else
        {
            // This scenario should only come up if you need to add a note before the first note of a chart
            enote->next = editor->current;
            enote->previous = editor->current->previous;

            editor->current->previous = enote;
            if(enote->previous != (void*)0)
            {
                enote->previous->next = enote;
            }
            else
            {
                editor->start = enote;
            }
        }
    }
    else
    {
        // If there is no current note, there shouldn't be any start or end notes aswell
        // Checks are mostlikely redundant
        //TraceLog(LOG_INFO, "current doesn't exists");
        if(editor->start == (void*)0)
        {
            editor->start = enote;
        }
        if(editor->end == (void*)0)
        {
            editor->end = enote;
        }
    }

    editor->current = enote;
    editor->current_time = enote->note.time;
    return OK;
}

bool EditorRemoveNote(EditorChart* editor)
{
    // Removes current note
    if(editor->current == (void*)0)
    {
        return FAIL;
    }
    EditorNote* previous;
    EditorNote* next;
    previous = editor->current->previous;
    next = editor->current->next;
    free(editor->current);
    if(previous != (void*)0)
    {
        previous->next = next;
        editor->current = previous;
    }
    else
    {
        // If previous doesn't exist, next is chosen.
        // If next also doesn't exist, it's fine setting current to null
        editor->current = next;
        // Current note's previous was empty, it was at the start of the list, something need to be put instead of it
        editor->start = next;
    }
    if(next != (void*)0)
    {
        next->previous = previous;
    }
    else
    {
        // Current note's next was empty, it was at the end if the list, something needs to be put instead of it
        editor->end = previous;
    }
    return OK; 
}

bool EditorClearNotes(EditorChart* editor)
{
    editor->current = editor->start;
    while(editor->current != (void*)0)
    {
        EditorNote* next = editor->current->next;
        free(editor->current);
        editor->current = next;
    }
    editor->start = (void*)0;
    editor->end = (void*)0;
    EditorMoveToStart(editor);
    return OK;
}

bool EditorMoveCurrentNote(EditorChart* editor, int time)
{
    EditorNote* current = editor->current;
    if(current == (void*)0)
    {
	return FAIL;
    }
    current->note.time += time;
    if(current->note.hold)
    {
	current->note.time_end += time;
    }
    EditorNote* previous = current->previous;
    if(previous != (void*)0)
    {
	if(current->note.time < previous->note.time)
	{
	    current->previous = previous->previous;
	    previous->previous = current;
	    previous->next = current->next;
	    current->next = previous;
	    if(current->previous == (void*)0)
	    {
		editor->start = current;
	    }
	    if(previous->next == (void*)0)
	    {
		editor->end = previous;
	    }
	}
    }
    EditorNote* next = current->next;
    if(next != (void*)0)
    {
	if(current->note.time > next->note.time)
	{
	    current->next = next->next;
	    next->next = current;
	    next->previous = current->previous;
	    current->previous = next;
	    if(current->next == (void*)0)
	    {
		editor->end = current;
	    }
	    if(next->previous == (void*)0)
	    {
		editor->start = next;
	    }
	}
    }
    editor->current_time = current->note.time;
    return OK;
}

bool EditorColorCurrentNote(EditorChart* editor, char color)
{
    if(editor->current != (void*)0)
    {
	editor->current->note.color = color;
	return OK;
    }
    return FAIL;
}

void EditorDefault(EditorChart* editor)
{
    EditorClearNotes(editor);
    *editor = (EditorChart){0};
    editor->note = (Note){0};
    editor->current_time = 0;
    editor->difficulty = 0;
    editor->speed = 0;
    editor->move_timer = 0;
    editor->current_color = 1;
    editor->mode = EM_MAIN;
    editor->edit_duration = false;
}

Chart* EditorToChart(EditorChart* editor)
{
    TraceLog(LOG_INFO, "Converting EditorChart to Chart");
    int code_amount = 0;
    EditorNote* current = editor->start;
    while(current != (void*)0)
    {
        if(current->note.hold)
        {
            code_amount += 2;
        }
        else
        {
            code_amount += 1;
        }
        current = current->next;
    }

    Chart* chart = malloc(sizeof(Chart) + sizeof(int) * code_amount);
    chart->difficulty = editor->difficulty;
    chart->speed = editor->speed;
    chart->code_amount = code_amount;

    current = editor->start;
    code_amount = 0;
    while(current != (void*)0)
    {
        int code = NoteToInt(current->note);
        chart->codes[code_amount] = code;
        code_amount++;
        if(current->note.hold)
        {
            code = code & 0b11111111;
            code += current->note.time_end << TIMECODE_SHIFT;
            chart->codes[code_amount] = code;
            code_amount++;
        }

        current = current->next;
    }

    return chart;
}

bool ChartToEditor(Chart* chart, EditorChart* editor)
{
    TraceLog(LOG_INFO, "Converting Chart to EditorChart");
    int i = 0;
    while(i < chart->code_amount)
    {
        Note note = IntToNote(chart->codes[i]);
        i++;
        if(note.hold)
        {
            note.time_end = chart->codes[i] >> TIMECODE_SHIFT;
            i++;
        }
        EditorAddNote(editor, note);
    }
    editor->difficulty = chart->difficulty;
    editor->speed = chart->speed;
    EditorMoveToStart(editor);
    return OK;
}

EditorProcessCode EditorProcess(EditorChart* editor)
{
    extern Options options;
    
    EditorProcessCode process_code = EDITOR_CONTINUE;

    // TraceLog(LOG_INFO, "k:\t ");
    int input = GetKeyboardInput();
    if(editor->mode == EM_INSERT_NORMAL || editor->mode == EM_INSERT_MINE)
    {
        if(input)
        {
            editor->note = (Note){0};
            editor->note.active = true;
            editor->note.time = editor->current_time;
            editor->note.key = KeyboardToKeycode(input, options.bindings);
            if(editor->mode == EM_INSERT_MINE)
            {
                editor->note.mine = true;
                editor->note.color = 0;
            }
            else
            {
                editor->note.color = editor->current_color;
            }
            EditorAddNote(editor, editor->note);
            editor->note.active = false;
        }
    }
    if(editor->mode == EM_INSERT_HOLD)
    {
        if(input && !editor->note.active)
        {
            editor->note = (Note){0};
            editor->note.active = true;
            editor->note.time = editor->current_time;
            editor->note.time_end = editor->current_time;
            editor->note.key = KeyboardToKeycode(input, options.bindings);
            editor->note.color = editor->current_color;
            editor->note.hold = true;
        }
        else if(input && editor->note.active)
        {
            editor->note.time_end = editor->current_time;
            EditorAddNote(editor, editor->note);
            editor->note.active = false;
        }
    }
    if(editor->mode == EM_EDIT_NOTE)
    {
        if(editor->current != (void*)0 && input)
        {
            editor->current->note.key = KeyboardToKeycode(input, options.bindings);
        }
        if(IsKeyPressed(KEY_TAB))
        {
            editor->edit_duration = !editor->edit_duration;
            if(editor->edit_duration)
            {
                TraceLog(LOG_INFO, "tab:\t Edit Hold Duration");
            }
            else
            {
                TraceLog(LOG_INFO, "tab:\t Edit Note Time");
            }
        }
    }
    if(editor->mode == EM_MAIN)
    {
        if(IsKeyPressed(KEY_ENTER))
        {
            process_code = EDITOR_EXIT_PLAYTEST;
        }
        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_L))
        {
            TraceLog(LOG_INFO, "ctrl+l:\t Load Chart");
            process_code = EDITOR_LOAD_CHART;
        }
        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
        {
            TraceLog(LOG_INFO, "ctrl+s:\t Save Chart");
            process_code = EDITOR_SAVE_CHART;
        }
        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O))
        {
            TraceLog(LOG_INFO, "ctrl+o:\t Clear Chart");
            EditorClearNotes(editor);
        }
        if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_P))
        {
            TraceLog(LOG_INFO, "ctrl+p:\t Print Editor Notes");
            PrintEditor(editor);
        }
        if(IsKeyPressed(KEY_N))
        {
            TraceLog(LOG_INFO, "n:\t Mode INSERT NORMAL");
            editor->mode = EM_INSERT_NORMAL;
        }
        if(IsKeyPressed(KEY_H))
        {
            TraceLog(LOG_INFO, "h:\t Mode INSERT HOLD");
            editor->mode = EM_INSERT_HOLD;
        }
        if(IsKeyPressed(KEY_M))
        {
            TraceLog(LOG_INFO, "m:\t Mode INSERT MINE");
            editor->mode = EM_INSERT_MINE;
        }
        if(IsKeyPressed(KEY_E))
        {
            TraceLog(LOG_INFO, "e:\t Mode EDIT NOTE");
            editor->mode = EM_EDIT_NOTE;
        }
    }
    if(editor->mode == EM_EXIT)
    {
        if(IsKeyPressed(KEY_ENTER))
        {
            process_code = EDITOR_EXIT_MENU;
        }
    }
    // GENERIC EDITOR
    if(IsKeyPressed(KEY_BACKSPACE))
    {
        EditorRemoveNote(editor);
        /*
        if(editor->mode == EM_EDIT_NOTE)
        {
            TraceLog(LOG_INFO, "Exited EDIT NOTE to MAIN");
            editor->mode = EM_MAIN;
        }
        */
    }
    // current_color = 0 is reserved for mines
    const int COLOR_KEYS[] = {KEY_ONE, KEY_TWO, KEY_THREE};
    for(int i = 0; i < 3; i++)
    {
        if(IsKeyPressed(COLOR_KEYS[i]))
        {
            editor->current_color = i + 1;
            TraceLog(LOG_INFO, "%i:\t Color %i", editor->current_color, editor->current_color);
            if(editor->mode == EM_EDIT_NOTE) EditorColorCurrentNote(editor, editor->current_color);
        }
    }

    bool left_pressed = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_COMMA);
    bool right_pressed = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_PERIOD);

    if(IsKeyDown(KEY_LEFT_CONTROL) && editor->mode != EM_EDIT_NOTE)
    {
        if(left_pressed)
        {
            TraceLog(LOG_INFO, "ctrl+left:\t First Note / Start");
            EditorMoveToStart(editor);
        }
        if(right_pressed)
        {
            TraceLog(LOG_INFO, "ctrl+right:\t Last Note / End");
            EditorMoveToEnd(editor);
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

        if(editor->mode == EM_EDIT_NOTE && editor->edit_duration)
        {
            if(editor->current != (void*)0 && editor->current->note.hold)
            {
                if(left_pressed && InputTiming(&editor->move_timer, false))
                {
                    editor->current->note.time_end -= speed;
                    if(editor->current->note.time_end < 0)
                    {
                        editor->current->note.time_end = 0;
                    }
                }
                if(right_pressed && InputTiming(&editor->move_timer, false))
                {
                    editor->current->note.time_end += speed;
                }
            }
        }
        else if(editor->mode == EM_EDIT_NOTE)
        {
            if(left_pressed && InputTiming(&editor->move_timer, false))
            {
                EditorMoveCurrentNote(editor, -speed);
            }
            if(right_pressed && InputTiming(&editor->move_timer, false))
            {
                EditorMoveCurrentNote(editor, speed);
            }
        }
        else
        {
            if(left_pressed)
            {
                EditorMoveTimed(editor, -speed);
            }
            if(right_pressed)
            {
                EditorMoveTimed(editor, speed);
            }
        }
    }
    if(IsKeyDown(KEY_UP) && InputTiming(&editor->move_timer, false))
    {
        EditorMoveToNext(editor);
    }
    if(IsKeyDown(KEY_DOWN) && InputTiming(&editor->move_timer, false))
    {
        EditorMoveToPrevious(editor);
    }
    if(!left_pressed && !right_pressed && !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
    {
        InputTiming(&editor->move_timer, true);
    }
    if(IsKeyPressed(KEY_ESCAPE))
    {
        if(editor->mode == EM_MAIN)
        {
            TraceLog(LOG_INFO, "esc:\t Exit Attempt");
            editor->mode = EM_EXIT;
        }
        else
        {
            TraceLog(LOG_INFO, "esc:\t Mode MAIN");
            editor->mode = EM_MAIN;
            editor->note.active = false;
        }
    }

    return process_code;
}

void PrintEditor(EditorChart* editor)
{
    Note note;
    if(editor->start != (void*)0)
    {
        note = editor->start->note;
        TraceLog(LOG_INFO, "start: h%i m%i k%i t%i", note.hold, note.mine, note.key, note.time);
    }
    EditorNote* current = editor->start;
    int i = 0;
    while(current != (void*)0)
    {
        note = current->note;
        TraceLog(LOG_INFO, "%i, h%i m%i k%i t%i", i, note.hold, note.mine, note.key, note.time);
        current = current->next;
        i++;
    }
    if(editor->end != (void*)0)
    {
        note = editor->end->note;
        TraceLog(LOG_INFO, "end: h%i m%i k%i t%i", note.hold, note.mine, note.key, note.time);
    }
}

void DebugDrawEditor(EditorChart* editor)
{
    //TraceLog(LOG_INFO, "Debug Draw Editor");
    if(editor->note.active)
    {
        DebugDrawNoteOutline(editor->note, BLUE);
        DrawText(TextFormat("(%i)", editor->note.time), 96, 64, 24, BLUE);
    }
    if(editor->edit_duration && editor->current != (void*)0)
    {
        DrawText(TextFormat("(%i)", editor->current->note.time_end), 96, 64, 24, BLUE);
    }

    if(editor->current != (void*)0)
    {
        DebugDrawNoteOutline(editor->current->note, BLACK);
    }

    EditorNote* current = editor->start;
    int i = 0;
    while(current != (void*)0)
    {
        //TraceLog(LOG_INFO, "%i", i);
        /*
        int x = 2 + i * 72;
        DrawText(TextFormat("%i", current), x, 32, 8, BLACK);
        DrawText(TextFormat("> %i", current->next), x, 40, 8, BLACK);
        DrawText(TextFormat("< %i", current->previous), x, 48, 8, BLACK);
        */
        Note note = current->note;
        if(note.hold)
        {
            int diff = note.time - editor->current_time;
            int diff2 = editor->current_time - note.time_end;
            if(diff < NOTE_SPAWN_WINDOW && diff2 < NOTE_DESPAWN_WINDOW)
            {
                DebugDrawNote(note, editor->current_time);
            }
        }
        else
        {
            int diff = note.time - editor->current_time;
            int diff2 = editor->current_time - note.time;
            if(diff < NOTE_SPAWN_WINDOW && diff2 < NOTE_DESPAWN_WINDOW)
            {
                DebugDrawNote(note, editor->current_time);
            }
        }

        current = current->next;
        i++;
    }
    DrawText("EDITOR", 32, 32, 24, BLACK);
    DrawText(TextFormat("%i", editor->current_time), 32, 64, 24, BLACK);
}
