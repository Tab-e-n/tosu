class_name NoteHold
extends Note
# Basic note that the player has to hold

#@export var key_number : int = 0
@export var end_time : float = 3
#@export var pallete : int = 0

#var key : String = "Q"
var done : bool = false
#var appearence : float = 0
#var disappearence : float = 0
var failed_holding : bool = false
var note_combined_points : int = 0
var hit : bool = false


@onready var main : Label = Label.new()
@onready var border : Sprite2D = Sprite2D.new()
@onready var inside : Sprite2D = Sprite2D.new()
#@onready var root : Node = get_tree().current_scene
@onready var duration : SquareSpinner = SquareSpinner.new()

@onready var note_end : NoteHoldEnd = NoteHoldEnd.new()
@onready var note_start : NoteHoldStart = NoteHoldStart.new()

func _note_ready():
	inside.texture = preload("res://HoldInside.png")
	inside.position = Vector2(48, 48)
	inside.modulate = root.note_palletes[pallete]
	inside.modulate.a = 0
	add_child(inside)
	
	border.texture = preload("res://HoldBorder.png")
	border.position = Vector2(48, 48)
	#border.modulate = root.note_palletes[pallete]
	border.modulate.a = 0
	add_child(border)
	
	duration.time = 1
	duration.scale = Vector2(5.0 / 6.0, 5.0 / 6.0)
	duration.position = Vector2(8, 8)
	duration.modulate = root.note_palletes[pallete]
	duration.modulate.a = 0
	add_child(duration)
	
	#main
	main.text = key
	main.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	main.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	main.theme = preload("res://default_theme.tres")
	main.clip_text = true
	main.size = Vector2(96, 96)
	#main.modulate = root.note_palletes[pallete]
	main.modulate.a = 0
	add_child(main)
	
	note_start.key_number = key_number
	note_start.hit_time = hit_time
	note_start.pallete = pallete
	note_start.botplay = botplay
	add_child(note_start)
	
	note_end.key_number = key_number
	note_end.hit_time = end_time
	note_end.pallete = pallete
	note_end.botplay = botplay
	add_child(note_end)

func _process(delta):
	if root.timer < hit_time:
		appearence = root.appearence_equation(hit_time)
		_start_interval(appearence)
		
		if appearence < 0:
			queue_free()
	elif root.timer < end_time:
		_middle_interval()
		
		if !botplay and !failed_holding and note_start.appearence < 1 - root.INPUT_DROP_TIME and note_end.appearence < 1 - root.INPUT_DROP_TIME:
			var input : InputEventKey = InputEventKey.new()
			var input_released_exists : bool = false
			var pops : int = 0
			for i in root.inputs.size():
				if root.inputs[i - pops].as_text() == key and !root.inputs[i - pops].pressed:
					input = root.inputs.pop_at(i - pops)
					pops += 1
					input_released_exists = true
					break;
			
			if input_released_exists:
				note_pressed(0)
				failed_holding = true
	if root.timer > end_time:
		if !botplay and !done and hit:
			if !failed_holding and note_combined_points > 0:
				note_pressed(20)
			var average : int = note_combined_points / 3
			#print("avr: ", average)
			if average < 2.5:
				average = 0
			elif average < 7.5:
				average = 5
			elif average <= 20:
				average = 10
			else:
				average = 30
			done = true
			root.note_pressed(average, position + Vector2(48, 48))
		disappearence = root.disappearecne_equation(end_time)
		_end_interval(disappearence)
	
	if disappearence >= 1:
		queue_free()
	
	
	z_index += 1
	
	#if input_pressed_exists:
		#_hit()
		#if appearence > 1 - root.HIT_300:
			#_hit_300()
		#elif appearence > 1 - root.HIT_100:
			#_hit_100()
		#elif appearence > 1 - root.HIT_50:
			#_hit_50()
		#else:
			#_hit_0()
		#pressed = true
	
	#if input_pressed_exists:
		#_released()
		#if appearence > 1 - hit_300:
			#_released_300()
		#elif appearence > 1 - hit_100:
			#_released_100()
		#elif appearence > 1 - hit_50:
			#_released_50()
		#else:
			#_released_0()
		#pressed = true

func _start_interval(time : float):
	inside.modulate.a = time
	border.modulate.a = time
	main.modulate.a = time
	
	duration.time = 0
	duration.modulate.a = 0.6 * time
	#print("start ", time)

func _middle_interval():
	inside.modulate.a = 1
	border.modulate.a = 1
	main.modulate.a = 1
	duration.modulate.a = 0.6
	
	duration.time = (root.timer - hit_time) / (end_time - hit_time)
	#print("middle ", time)

func _end_interval(time : float):
	inside.modulate.a = 1 - time
	border.modulate.a = 1 - time
	main.modulate.a = 1 - time
	
	duration.time = 1
	duration.modulate.a = 0.6 * (1 - time)
	#print("end ", time)

func note_pressed(points : int, is_end : bool = false):
	note_combined_points += points
	if is_end:
		hit = true
