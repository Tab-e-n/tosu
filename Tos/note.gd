class_name Note
extends Node2D
# Basic note that the player has to hit


@export var key_number : int = 0
@export var hit_time : float = 2
@export var pallete : int = 0
@export var should_input_be_pressed = true
@export var should_delete_self : bool = true
@export var botplay : bool = false

var key : String = "Q"
var appearence : float = 0
var disappearence : float = 0
var pressed : bool = false
var pressed_time : float = 0

@onready var root : Node = get_tree().current_scene

func _ready():
	key = root.note_letters[key_number]
	
	position = root.note_positions[key_number]
	
	z_index = -100
	
	_note_ready()

func _note_ready():
	pass

func _process(delta):
	appearence = root.appearence_equation(hit_time)
	
	if appearence <= 1 and root.timer < hit_time:
		_start_interval(appearence)
	
	if !botplay:
		if appearence < 1 - root.INPUT_DROP_TIME and root.timer > hit_time:
			if !pressed:
				pressed = true
				pressed_time = root.timer
				_miss()
		
		if !pressed and root.timer > hit_time:
			_middle_interval()
		
		if pressed:
			disappearence = root.disappearecne_equation(pressed_time)
			_end_interval(disappearence)
	else:
		if root.timer == hit_time:
			_middle_interval()
		if root.timer > hit_time:
			disappearence = root.disappearecne_equation(hit_time)
			_end_interval(disappearence)
	
	if disappearence >= 1 and should_delete_self:
		queue_free()
	
	if appearence < 0 and should_delete_self:
		queue_free()
	
	z_index += 1
	
	if !botplay:
		var input : InputEventKey = InputEventKey.new()
		var input_pressed_exists : bool = false
		var input_released_exists : bool = false
		if !pressed and appearence > 1 - root.INPUT_DROP_TIME:
			var pops : int = 0
			for i in root.inputs.size():
				if root.inputs[i - pops].as_text() == key and root.inputs[i - pops].pressed == should_input_be_pressed:
					input = root.inputs.pop_at(i - pops)
					pops += 1
					if should_input_be_pressed:
						input_pressed_exists = true
					else:
						input_released_exists = true
					break;
		
		if input_pressed_exists:
			_hit()
			if appearence > 1 - root.HIT_300:
				_hit_300()
			elif appearence > 1 - root.HIT_100:
				_hit_100()
			elif appearence > 1 - root.HIT_50:
				_hit_50()
			else:
				_hit_0()
			pressed = true
			pressed_time = root.timer
		
		if input_released_exists:
			_released()
			if appearence > 1 - root.HIT_300:
				_released_300()
			elif appearence > 1 - root.HIT_100:
				_released_100()
			elif appearence > 1 - root.HIT_50:
				_released_50()
			else:
				_released_0()
			pressed = true
			pressed_time = root.timer

func _start_interval(appearence : float):
	pass

func _middle_interval():
	pass

func _end_interval(disappearence : float):
	pass

func _hit():
	pass
	
func _hit_300():
	pass

func _hit_100():
	pass

func _hit_50():
	pass

func _hit_0():
	pass

func _released():
	pass

func _released_300():
	pass
#aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaahoj
func _released_100():
	pass

func _released_50():
	pass

func _released_0():
	pass

func _miss():
	pass
