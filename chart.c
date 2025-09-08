#include <stdlib.h>
#include <raylib.h>
#include "notes.h"
#include "chart.h"

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
			note.time_end = chart->codes[game->current_code] >> 8;
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
		int delta = (chart->codes[game->current_code] >> 8) - game->time;
		TraceLog(LOG_INFO, "%i", delta);
		return delta < NOTE_SPAWN_WINDOW;
	}
	return false;
}

ChartLoadResult LoadChart(const char* filename)
{
	int size;
	unsigned char* file = LoadFileData(filename, &size);
	ChartLoadResult result = (ChartLoadResult){0};
	
	if(file != (void*)0)
	{
		Chart* chart = (Chart*)malloc(size);
		*chart = *(Chart*)file;
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
				*save_chart = *chart;
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
			*save_chart = *chart;
		}

		success = SaveFileData(filename, save_file, save_size);
		UnloadFileData(save_file);
	}
	else
	{
		data_size = SizeOfChart(chart);
		data_file = (unsigned char*)malloc(data_size);

		Chart* save_chart = (Chart*)(save_file);
		*save_chart = *chart;

		success = SaveFileData(filename, save_file, save_size);
		UnloadFileData(save_file);
	}

	return success;
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
		while(current->next != (void*)0 && current->next->note.time < editor->current_time)
		{
			current = current->next;
		}
	}
	if(time < 0 && current->previous != (void*)0)
	{
		while(current->previous != (void*)0 && current->previous->note.time > editor->current_time)
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
		// Current note's next was empty, it was at the end if the list, something need to be put instead of it
		editor->end = previous;
	}
	return OK; 
}

bool EditorClearNotes(EditorChart* editor)
{
	EditorMoveToStart(editor);
	editor->start = (void*)0;
	editor->end = (void*)0;
	while(editor->current != (void*)0)
	{
		EditorNote* next = editor->current->next;
		free(editor->current);
		editor->current = next;
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
	note.active = true;
	return note;
}

void DebugDrawEditor(EditorChart* editor)
{
	//TraceLog(LOG_INFO, "Debug Draw Editor");
	const int NOTE_CUTOFF = 60;
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
			if(diff < NOTE_CUTOFF && diff2 < NOTE_CUTOFF)
			{
				DebugDrawNote(note, editor->current_time);
			}
		}
		else
		{
			int diff = note.time - editor->current_time;
			diff = diff < 0 ? -diff : diff;
			if(diff < NOTE_CUTOFF)
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
