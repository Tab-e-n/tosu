#ifndef TOSU_CHART
#define TOSU_CHART

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
	int note_amount;
	int notes[];
} Chart;

typedef struct EditorNote
{
	EditorNote* previous, next;
	Note note;
} EditorNote;

typedef struct EditorChart
{
	EditorNote* start, end, current;
	int current_time. note_amount;
	char difficulty, speed;
} EditorChart;


bool ChartReadNext(Chart* chart, GameSpace game);

Chart* LoadChart(const char* filename);
bool SaveChart(Chart* chart, const char* filename);


bool EditorMoveToStart(EditorChart* editor); // Sets current to start
bool EditorMoveToEnd(EditorChart* editor); // Sets current to end
bool EditorMove(EditorChart* editor, int time); // Increases current_time, changes current_note if required
bool EditorMoveToTimecode(EditorChart* editor, int timecode); // Sets current_time to timecode, changes current_note if required

bool EditorAddNote(EditorChart* editor, Note note);
bool EditorRemoveNote(EditorChart* editor);

Chart* EditorToChart(EditorChart* editor);
EditorChart* ChartToEditor(Chart* chart);



#endif
