class_name Note
extends Node2D
# Basic note that the player has to hit


var hit_300 : float = 0.080
var hit_100 : float = 0.160
var hit_50 : float = 0.200
var input_drop_time : float = 0.280

@export var key_number : int = 0
@export var hit_time : float = 2
@export var pallete : int = 0
@export var should_input_be_pressed = true

var key : String = "Q"
var appearence : float = 0
var disappearence : float = 0
var pressed : bool = false

@onready var parent : Node = get_parent()

func _ready():
	hit_300 -= 0.004 * parent.difficulty
	hit_100 -= 0.008 * parent.difficulty
	hit_50 -= 0.010 * parent.difficulty
	input_drop_time -= 0.014 * parent.difficulhty
	
	key = parent.note_letters[key_number]
	
	position = parent.note_positions[key_number]
	
	z_index = -100
	
	_note_ready()

func _note_ready():
	pass

func _process(delta):
	appearence = 1 - abs(parent.timer - hit_time) * (1 + parent.approach / 4)
	
	if appearence <= 1 and parent.timer < hit_time:
		_start_interval(appearence)
	
	if appearence < 1 - input_drop_time and parent.timer > hit_time:
		if !pressed:
			pressed = true
			_miss()
	
	if pressed:
		disappearence += delta * 4
		_end_interval(disappearence)
	
	if disappearence >= 1:
		queue_free()
	
	z_index += 1
	
	var input : InputEventKey = InputEventKey.new()
	var input_pressed_exists : bool = false
	var input_released_exists : bool = false
	if !pressed and appearence > 1 - input_drop_time:
		var pops : int = 0
		for i in parent.inputs.size():
			if parent.inputs[i - pops].as_text() == key and parent.inputs[i - pops].pressed == should_input_be_pressed:
				input = parent.inputs.pop_at(i - pops)
				pops += 1
				if should_input_be_pressed:
					input_pressed_exists = true
				else:
					input_released_exists = true
				break;
	
	if input_pressed_exists:
		_hit()
		if appearence > 1 - hit_300:
			_hit_300()
		elif appearence > 1 - hit_100:
			_hit_100()
		elif appearence > 1 - hit_50:
			_hit_50()
		else:
			_hit_0()
		pressed = true
	
	if input_pressed_exists:
		_released()
		if appearence > 1 - hit_300:
			_released_300()
		elif appearence > 1 - hit_100:
			_released_100()
		elif appearence > 1 - hit_50:
			_released_50()
		else:
			_released_0()
		pressed = true

func _start_interval(appearence : float):
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
