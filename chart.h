#ifndef TOSU_CHART
#define TOSU_CHART

#include <stdlib.h>
#include <raylib.h>
#include "notes.h"


#define OK 0
#define ERROR 1
#define UNIMPLEMENTED 2
// Might do more specific error codes if necessary

/*
shmccccc tttttttt tttttttt tttttttt

s - Unused bit (will probably be used for non-note objects)
h - Is note a hold note
m - Is note a mine
c - The notes key
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
	char difficulty, speed;
} EditorChart;


int SizeOfChart(Chart* chart);
bool ChartReadNext(Chart* chart, GameSpace* game);

ChartLoadResult LoadChart(const char* filename);
bool SaveChart(Chart* chart, const char* filename);

bool EditorMoveToStart(EditorChart* editor); // Sets current to start
bool EditorMoveToEnd(EditorChart* editor); // Sets current to end
bool EditorMove(EditorChart* editor, int time); // Increases current_time, changes current_note if required
bool EditorMoveToTimecode(EditorChart* editor, int timecode); // Sets current_time to timecode, changes current_note if required

bool EditorAddNote(EditorChart* editor, Note note);
bool EditorRemoveNote(EditorChart* editor);
bool EditorClearNotes(EditorChart* editor);

Chart* EditorToChart(EditorChart* editor);
bool ChartToEditor(Chart* chart, EditorChart* editor);

int NoteToInt(Note note);
Note IntToNote(int code);

void DebugDrawEditor(EditorChart* editor);

#endif
