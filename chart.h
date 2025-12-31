#ifndef TOSU_CHART
#define TOSU_CHART

#include <stdlib.h>
#include <raylib.h>
#include "options.h"
#include "notes.h"


#define MAX_FILENAME_CHARACTERS 100
// 6 for ".chart" + 1 for "\0"
#define FILENAME_CHARACTERS_INTERNAL (MAX_FILENAME_CHARACTERS + 7)

#define MOVE_DELAY_FRAMES 5
#define TIMECODE_SHIFT 10


typedef enum EditorMode {EM_MAIN, EM_INSERT_NORMAL, EM_INSERT_HOLD, EM_INSERT_MINE, EM_EDIT_NOTE, EM_EXIT} EditorMode;
typedef enum EditorProcessCode {
    EDITOR_CONTINUE, 
    EDITOR_EXIT_PLAYTEST, EDITOR_EXIT_MENU, 
    EDITOR_LOAD_CHART, EDITOR_SAVE_CHART
} EditorProcessCode;

/*
shmkkkkk cctttttt tttttttt tttttttt

s - Unused bit (will probably be used for non-note objects)
h - Is note a hold note
m - Is note a mine
k - The notes key
c - The notes color
t - The notes timecode
*/

typedef struct Chart
{
    char difficulty, speed;
    int code_amount;
    int codes[];
} Chart;

typedef struct ChartLoadResult
{
    bool success;
    Chart* chart;
} ChartLoadResult;

typedef struct ChartFilename
{
    int current;
    char str[FILENAME_CHARACTERS_INTERNAL];
} ChartFilename;

typedef struct EditorNote
{
    struct EditorNote* previous;
    struct EditorNote* next;
    Note note;
} EditorNote;

typedef struct EditorChart
{
    EditorNote* start;
    EditorNote* end;
    EditorNote* current;
    Note note;
    int current_time;
    char difficulty, speed,
         move_timer, current_color;
    EditorMode mode;
    bool edit_duration;
} EditorChart;


int NoteToInt(Note note);
Note IntToNote(int code);

int SizeOfChart(Chart* chart);
bool ChartReadNext(Chart* chart, GameSpace* game);
bool ChartShouldReadNext(Chart* chart, GameSpace* game);

bool CopyChart(Chart* copy, Chart* chart);
ChartLoadResult LoadChart(const char* filename);
bool SaveChart(Chart* chart, const char* filename);

void ChartFilenameClear(ChartFilename* filename);
bool ChartFilenameAddChar(ChartFilename* filename, char c);
bool ChartFilenameRemoveChar(ChartFilename* filename);
void ChartFilenameSuffix(ChartFilename* filename);
void ChartFilenameRemoveSuffix(ChartFilename* filename);
bool ChartFilenameFill(ChartFilename* filename, const char* str);

bool EditorMoveToStart(EditorChart* editor); // Sets current note to start
bool EditorMoveToEnd(EditorChart* editor); // Sets current note to end
bool EditorMoveToNext(EditorChart* editor);
bool EditorMoveToPrevious(EditorChart* editor);

bool EditorMove(EditorChart* editor, int time); // Increases current_time, changes current note if required
bool EditorMoveToTimecode(EditorChart* editor, int timecode); // Sets current_time to timecode, changes current note if required
bool EditorMoveTimed(EditorChart* editor, int time);

bool EditorAddNote(EditorChart* editor, Note note);
bool EditorRemoveNote(EditorChart* editor);
bool EditorClearNotes(EditorChart* editor);

bool EditorMoveCurrentNote(EditorChart* editor, int time);
bool EditorColorCurrentNote(EditorChart* editor, char color);

void EditorDefault(EditorChart* editor);
Chart* EditorToChart(EditorChart* editor);
bool ChartToEditor(Chart* chart, EditorChart* editor);

EditorProcessCode EditorProcess(EditorChart* editor);
void PrintEditor(EditorChart* editor);
void DebugDrawEditor(EditorChart* editor);

#endif
