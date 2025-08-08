#include <stdlib.h>
#include <raylib.h>
#include "notes.h"
#include "chart.h"

bool ChartReadNext(Chart* chart, GameSpace game)
{
	return UNIMPLEMENTED;
}

Chart* LoadChart(const char* filename)
{
	return (void*)0;
}

bool SaveChart(Chart* chart, const char* filename)
{
	return UNIMPLEMENTED;
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
	return ERROR;
}

bool EditorMoveToEnd(EditorChart* editor)
{
	editor->current = editor->end;
	if(editor->current != (void*)0)
	{
		editor->current_time = editor->current->note.time;
		return OK;
	}
	return ERROR;
}

bool EditorMove(EditorChart* editor, int time)
{
	editor->current_time += time;
	if(editor->current == (void*)0)
	{
		return ERROR;
	}
	EditorNote* current = editor->current;
	if(time > 0 && current->next != (void*)0)
	{
		while(current->next != (void*)0 && current->next->note.time < editor->current_time)
		{
			editor->current = current->next;
		}
	}
	if(time < 0 && current->previous != (void*)0)
	{
		while(current->previous != (void*)0 && current->previous->note.time > editor->current_time)
		{
			editor->current = current->previous;
		}
	}
	return OK;
}

bool EditorMoveToTimecode(EditorChart* editor, int timecode)
{
	return EditorMove(editor, timecode - editor->current_time);
}

bool EditorAddNote(EditorChart* editor, Note note)
{
	EditorNote* enote = malloc(sizeof(EditorNote));
	if(enote == (void*)0)
	{
		return ERROR;
	}
	enote->note = note;

	if(editor->current != (void*)0)
	{
		if(editor->current->note.time <= enote->note.time)
		{
			enote->previous = editor->current;
			enote->next = editor->current->next;

			editor->current->next = enote;
			if(enote->next != (void*)0)
			{
				enote->next->previous = enote;
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
		}
	}
	else
	{
		// If there is no current note, there shouldn't be any start or end notes aswell
		// Checks are mostlikely redundant, maybe even detrimental?
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
		return ERROR;
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
		// Current note's next was empty, it was at the end if the list, something need to be put instead of it
		editor->end = previous;
	}
	return OK; 
}

Chart* EditorToChart(EditorChart* editor)
{
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
			code += current->note.time_end << 8;
			chart->codes[code_amount] = code;
			code_amount++;
		}

		current = current->next;
	}

	return chart;
}

bool ChartToEditor(Chart* chart, EditorChart* editor)
{
	int i = 0;
	while(i < chart->code_amount)
	{
		Note note = IntToNote(chart->codes[i]);
		i++;
		if(note.hold)
		{
			note.time_end = chart->codes[i] >> 8;
			i++;
		}
		EditorAddNote(editor, note);
	}
	editor->difficulty = chart->difficulty;
	editor->speed = chart->speed;
	return OK;
}

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
	code += note.time << 8;
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
	note.time = code >> 8;
	return note;
}

