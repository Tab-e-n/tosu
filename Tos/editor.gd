extends NoteControl

enum {
	MODE_ADD_SINGLE_NOTE,
	MODE_COMMANDS,
	MODE_JUMP,
	MODE_ADD_MANY_NOTES,
	MODE_ADD_MINE,
	MODE_ADD_MANY_MINES,
	MODE_ADD_HOLD,
	MODE_ADD_MANY_HOLDS,
	MODE_CHANGE_BPM,
	MODE_CHANGE_APPROACH,
	MODE_CHANGE_DIFFICULTY,
	MODE_SAVE,
	MODE_LOAD,
	MODE_REMOVE_NOTES,
}

@export var song_offset : float = 0

var mode : int = MODE_COMMANDS
var command_argument : String = ""
var bpm_period : float = 0
var current_pallete : int = 0

var adding_hold_note : bool = false
var hold_note_timer : float = 0
var hold_note_key : int = 0

func _ready():
	botplay = true
	
	calculate_bpm_period()
	$bpm.text = String.num(bpm)
	$diff.text = String.num(difficulty)
	$app.text = String.num(approach)
	#print(notes)

func change_timer(delta : float):
	var time_warp : float = 1
	var shift : bool = Input.is_action_pressed("shift")
	var ctrl : bool = Input.is_action_pressed("ctrl")
	var alt : bool = Input.is_action_pressed("alt")
	
	# Holding shift makes you go faster, jump farther
	if shift:
		time_warp = 4
	# Basic movement back and forth
	if Input.is_action_pressed("left") and !ctrl and !alt:
		timer -= delta * time_warp
	if Input.is_action_pressed("right") and !ctrl and !alt:
		timer += delta * time_warp
	# Jumping back and forth
	if Input.is_action_just_pressed("skip_backward") and !alt:
		timer -= 10 * time_warp
	if Input.is_action_just_pressed("skip_foward") and !alt:
		timer += 10 * time_warp
	# Jumping between beats
	var shifted_period = bpm_period * time_warp
	if ctrl:
		shifted_period = shifted_period / 4
	if Input.is_action_just_pressed("bpm_shift_backward"):
		var period_amount : float = round((timer - song_offset - shifted_period) / shifted_period)
		timer = song_offset + period_amount * shifted_period
	if Input.is_action_just_pressed("bpm_shift_foward"):
		var period_amount : float = round((timer - song_offset + shifted_period) / shifted_period)
		timer = song_offset + period_amount * shifted_period
	# Jumping between notes
	if Input.is_action_just_pressed("up"):
		var closest_time : float = -1
		for i in notes.keys():
			if !i.is_valid_float():
				continue
			var hit_time = float(i)
			if hit_time < closest_time and hit_time > timer:
				closest_time = hit_time
				continue
			if closest_time == -1 and hit_time > timer:
				closest_time = hit_time
		if closest_time != -1:
			timer = closest_time
	if Input.is_action_just_pressed("down"):
		var closest_time : float = -1
		for i in notes.keys():
			if !i.is_valid_float():
				continue
			var hit_time = float(i)
			if hit_time > closest_time and hit_time < timer - 0.00001:
				closest_time = hit_time
				continue
			if closest_time == -1 and hit_time < timer:
				closest_time = hit_time
		if closest_time != -1:
			timer = closest_time
	
	timer = snapped(timer, 0.0001)
	if adding_hold_note and timer < hold_note_timer:
		timer = hold_note_timer
	$timer.text = String.num(timer)
	#print("main ", timer)

func check_and_create_notes():
	input_modeless()
	match mode:
		MODE_ADD_SINGLE_NOTE:
			$mode.text = "[center]ADD NOTE[/center]"
			input_mode_add_note()
		MODE_ADD_MANY_NOTES:
			$mode.text = "[center]ADD NOTES[/center]"
			input_mode_add_note()
		MODE_ADD_MINE:
			$mode.text = "[center]ADD MINE[/center]"
			input_mode_add_note(1)
		MODE_ADD_MANY_MINES:
			$mode.text = "[center]ADD MINES[/center]"
			input_mode_add_note(1)
		MODE_ADD_HOLD:
			$mode.text = "[center]ADD HOLD[/center]"
			input_mode_add_note(2)
		MODE_ADD_MANY_HOLDS:
			$mode.text = "[center]ADD HOLDS[/center]"
			input_mode_add_note(2)
		MODE_COMMANDS:
			$mode.text = ""
			input_mode_commands()
		MODE_JUMP:
			$mode.text = "[center]JUMP TO TIME[/center]"
			input_mode_jump()
		MODE_CHANGE_BPM:
			$mode.text = "[center]CHANGE BPM[/center]"
			input_mode_change_bpm()
		MODE_CHANGE_APPROACH:
			$mode.text = "[center]CHANGE APPROACH[/center]"
			input_mode_change_approach()
		MODE_CHANGE_DIFFICULTY:
			$mode.text = "[center]CHANGE DIFFICULTY[/center]"
			input_mode_change_difficulty()
		MODE_SAVE:
			$mode.text = "[center]SAVE CHART[/center]"
			input_mode_file(0)
		MODE_LOAD:
			$mode.text = "[center]LOAD CHART[/center]"
			input_mode_file(1)
		MODE_REMOVE_NOTES:
			$mode.text = "[center]REMOVE NOTES[/center]"
			input_mode_remove_notes()
	$hold_note.visible = adding_hold_note
	if adding_hold_note:
		$hold_note/duration.text = "[center]" + String.num(timer - hold_note_timer) + "[/center]"
	
	for i in notes.keys():
		var breakout : bool = !i.is_valid_float()
		for j in get_children():
			if breakout:
				break
			if j is Note:
				if j.hit_time == float(i):
					breakout = true
		if breakout:
			continue
		if notes[i][0] == 2:
			if timer >= float(i) - 1 and timer <= float(i) + notes[i][3] + 0.25:
				make_note(float(i), notes[i][0], notes[i][1], notes[i][2], notes[i][3])
		else:
			if timer >= float(i) - 1 and timer <= float(i) + 0.25:
				make_note(float(i), notes[i][0], notes[i][1], notes[i][2])
			#print(i)
			#notes.erase(i)

func input_mode_add_note(type : int = 0):
	if timer < 0:
		return
	
	var shifted_period = bpm_period
	
	var shift : bool = Input.is_action_pressed("shift")
	var ctrl : bool = Input.is_action_pressed("ctrl")
	var alt : bool = Input.is_action_pressed("alt")
	
	if shift and ctrl:
		shifted_period = bpm_period / 4
	elif shift:
		shifted_period = bpm_period * 2
	elif ctrl:
		shifted_period = bpm_period / 2
	for i in inputs.size():
#		print(OS.get_keycode_string(inputs[i].keycode))
		if !adding_hold_note:
			if OS.get_keycode_string(inputs[i].keycode) in note_letters and inputs[i].pressed:
				#print(note_letters.find(inputs[i].as_text()))
				if alt:
					for j in get_children():
						if j is Note:
							if j.hit_time == timer:
								j.queue_free()
								break
				else:
					while String.num(timer) in notes.keys():
						timer += 0.0001
				
				if type != 2:
					notes[String.num(timer)] = [type, note_letters.find(OS.get_keycode_string(inputs[i].keycode)), current_pallete]
					if mode == MODE_ADD_SINGLE_NOTE or mode == MODE_ADD_MINE:
						mode = MODE_COMMANDS
						break
					else:
						timer += shifted_period
				else:
					adding_hold_note = true
					hold_note_timer = timer
					hold_note_key = note_letters.find(OS.get_keycode_string(inputs[i].keycode))
					$hold_note.position = note_positions[hold_note_key]
		if inputs[i].keycode == KEY_ENTER:
			if !adding_hold_note:
				timer += shifted_period
			else:
				notes[String.num(hold_note_timer)] = [type, hold_note_key, current_pallete, timer - hold_note_timer]
				adding_hold_note = false
				if mode == MODE_ADD_HOLD:
					mode = MODE_COMMANDS
					break
				else:
					timer += shifted_period

func input_mode_commands():
	if Input.is_action_just_pressed("mode_add_note"):
		mode = MODE_ADD_SINGLE_NOTE
	if Input.is_action_just_pressed("mode_add_many_notes"):
		mode = MODE_ADD_MANY_NOTES
	if Input.is_action_just_pressed("mode_jump"):
		mode = MODE_JUMP
	if Input.is_action_just_pressed("mode_add_mine"):
		mode = MODE_ADD_MINE
	if Input.is_action_just_pressed("mode_add_many_mines"):
		mode = MODE_ADD_MANY_MINES
	if Input.is_action_just_pressed("mode_add_hold"):
		mode = MODE_ADD_HOLD
	if Input.is_action_just_pressed("mode_add_many_holds"):
		mode = MODE_ADD_MANY_HOLDS
	if Input.is_action_just_pressed("mode_change_bmp"):
		mode = MODE_CHANGE_BPM
	if Input.is_action_just_pressed("mode_change_approach"):
		mode = MODE_CHANGE_APPROACH
	if Input.is_action_just_pressed("mode_change_difficulty"):
		mode = MODE_CHANGE_DIFFICULTY
	if Input.is_action_just_pressed("mode_save"):
		mode = MODE_SAVE
	if Input.is_action_just_pressed("mode_load"):
		mode = MODE_LOAD
	if Input.is_action_just_pressed("mode_remove_notes"):
		mode = MODE_REMOVE_NOTES

func input_mode_jump():
	text_edit(["1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "."])
	$mode_extra.text = "[center]" + command_argument + "[/center]"
	if Input.is_action_just_pressed("enter"):
		if command_argument.count(".") > 1:
			$mode_extra.text = ""
			command_argument = ""
			mode = MODE_COMMANDS
		else:
			timer = float(command_argument)
			$mode_extra.text = ""
			command_argument = ""
			mode = MODE_COMMANDS

func input_mode_change_bpm():
	text_edit(["1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "."])
	$mode_extra.text = "[center]" + command_argument + "[/center]"
	if Input.is_action_just_pressed("enter"):
		if command_argument.count(".") > 1:
			$mode_extra.text = ""
			command_argument = ""
			mode = MODE_COMMANDS
		else:
			bpm = float(command_argument)
			calculate_bpm_period()
			$bpm.text = String.num(bpm)
			$mode_extra.text = ""
			command_argument = ""
			mode = MODE_COMMANDS

func input_mode_change_approach():
	text_edit(["1", "2", "3", "4", "5", "6", "7", "8", "9", "0"])
	$mode_extra.text = "[center]" + command_argument + "[/center]"
	if Input.is_action_just_pressed("enter"):
		var amount = float(command_argument)
		if amount > 10:
			amount = 10
		approach = amount
		$app.text = String.num(amount)
		$mode_extra.text = ""
		command_argument = ""
		mode = MODE_COMMANDS
		
func input_mode_change_difficulty():
	text_edit(["1", "2", "3", "4", "5", "6", "7", "8", "9", "0"])
	$mode_extra.text = "[center]" + command_argument + "[/center]"
	if Input.is_action_just_pressed("enter"):
		var amount = float(command_argument)
		if amount > 10:
			amount = 10
		difficulty = amount
		$diff.text = String.num(amount)
		$mode_extra.text = ""
		command_argument = ""
		mode = MODE_COMMANDS

func input_mode_file(interaction : int):
	text_edit([
			"A", "B", "C", "D", "E", "F",
			"G", "H", "I", "J", "K", "L", "M",
			"N", "O", "P", "Q", "R", "S",
			"T", "U", "V", "W", "X", "Y", "Z",
			"a", "b", "c", "d", "e", "f",
			"g", "h", "i", "j", "k", "l", "m",
			"n", "o", "p", "q", "r", "s",
			"t", "u", "v", "w", "x", "y", "z",
			"1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
			" ", "_",
	])
	$mode_extra.text = "[center]" + command_argument + "[/center]"
	if Input.is_action_just_pressed("enter"):
		if Chart.song_name != "":
			if interaction == 0:
				Chart.save_chart("user://" + Chart.song_name + "/" + command_argument, notes)
			else:
				Chart.load_chart("user://" + Chart.song_name + "/" + command_argument)
		$mode_extra.text = ""
		command_argument = ""
		mode = MODE_COMMANDS

func input_mode_remove_notes():
	for i in inputs.size():
		if OS.get_keycode_string(inputs[i].keycode) in note_letters and inputs[i].pressed:
			var remove_list : Array = []
			for j in get_children():
				if j is Note:
					if j.key == OS.get_keycode_string(inputs[i].keycode):
						remove_list.append(j)
						#j.queue_free()
						#break
			if remove_list.size() > 0:
				var closest_note : Note = remove_list[0]
				var closest_range : float = abs(closest_note.hit_time - timer)
				for j in range(remove_list.size() - 1):
					if abs(remove_list[j + 1].hit_time - timer) < closest_range:
						closest_note = remove_list[j + 1]
						closest_range = abs(closest_note.hit_time - timer)
				notes.erase(String.num(closest_note.hit_time))
				closest_note.queue_free()

func input_modeless():
	if Input.is_action_just_pressed("esc"):
		$mode_extra.text = ""
		command_argument = ""
		mode = MODE_COMMANDS
		adding_hold_note = false

func text_edit(text_allowed : Array):
	for i in inputs.size():
		if String.chr(inputs[i].unicode) in text_allowed and inputs[i].pressed:
			command_argument += String.chr(inputs[i].unicode)

func calculate_bpm_period():
	bpm_period = 60 / bpm

func accuracy_text(acc_percent):
	var acc_number : float = round(acc_percent * 10000) / 100
	#$accuracy.text = String.num(acc_number)
