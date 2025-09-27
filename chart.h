#ifndef TOSU_CHART
#define TOSU_CHART

#include <stdlib.h>
#include <raylib.h>
#include "notes.h"


#define OK 0
#define FAIL 1
#define UNIMPLEMENTED 2
#define ERROR 3
#define ERROR_CHART_NOT_LOADED 4
#define ERROR_GAMESPACE_NOT_LOADED 5
// Should move these into a separate file

#define MOVE_DELAY_FRAMES 15
#define TIMECODE_SHIFT 10

/*
shmkkkkk cctttttt tttttttt tttttttt

s - Unused bit (will probably be used for non-note objects)
h - Is note a hold note
m - Is note a mine
k - The notes key
t - The notes timecode
c - The notes color
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
    int current_time;
    char difficulty, speed, move_timer;
} EditorChart;


int SizeOfChart(Chart* chart);
bool ChartReadNext(Chart* chart, GameSpace* game);
bool ChartShouldReadNext(Chart* chart, GameSpace* game);

bool CopyChart(Chart* copy, Chart* chart);
ChartLoadResult LoadChart(const char* filename);
bool SaveChart(Chart* chart, const char* filename);

bool EditorMoveToStart(EditorChart* editor); // Sets current to start
bool EditorMoveToEnd(EditorChart* editor); // Sets current to end
bool EditorMoveToNext(EditorChart* editor);
bool EditorMoveToPrevious(EditorChart* editor);

bool EditorMove(EditorChart* editor, int time); // Increases current_time, changes current_note if required
bool EditorMoveToTimecode(EditorChart* editor, int timecode); // Sets current_time to timecode, changes current_note if required
bool EditorTiming(EditorChart* editor, bool reset);
bool EditorMoveTimed(EditorChart* editor, int time);

bool EditorAddNote(EditorChart* editor, Note note);
bool EditorRemoveNote(EditorChart* editor);
bool EditorClearNotes(EditorChart* editor);
bool EditorMoveCurrentNote(EditorChart* editor, int time);

Chart* EditorToChart(EditorChart* editor);
bool ChartToEditor(Chart* chart, EditorChart* editor);

int NoteToInt(Note note);
Note IntToNote(int code);

void PrintEditor(EditorChart* editor);
void DebugDrawEditor(EditorChart* editor);

#endif
