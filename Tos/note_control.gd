class_name NoteControl
extends Node2D


const note_letters : Array = [
	"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
	"A", "S", "D", "F", "G", "H", "J", "K", "L",
	"Z", "X", "C", "V", "B", "N", "M",
]
const note_positions : Array = [
	Vector2(24, 160),
	Vector2(136, 160),
	Vector2(248, 160),
	Vector2(360, 160),
	Vector2(472, 160),
	Vector2(584, 160),
	Vector2(696, 160),
	Vector2(808, 160),
	Vector2(920, 160),
	Vector2(1032, 160),
	
	Vector2(56, 272),
	Vector2(168, 272),
	Vector2(280, 272),
	Vector2(392, 272),
	Vector2(504, 272),
	Vector2(616, 272),
	Vector2(728, 272),
	Vector2(840, 272),
	Vector2(952, 272),
	
	Vector2(120, 384),
	Vector2(232, 384),
	Vector2(344, 384),
	Vector2(458, 384),
	Vector2(570, 384),
	Vector2(682, 384),
	Vector2(794, 384),
]

@export var botplay : bool = false

var timer : float = 0
var inputs : Array = []

var overall_points : int = 0
var point_amount : int = 0
var hit_note_amount : int = 0


@onready var notes : Dictionary = Chart.notes.duplicate()
@onready var note_palletes : Array = notes["note_palletes"]
@onready var difficulty : int = notes["difficulty"]
@onready var approach : int = notes["approach"]
@onready var bpm : float = notes["bpm"]

@onready var HIT_300 : float = 0.080 - 0.004 * difficulty
@onready var HIT_100 : float = 0.160 - 0.008 * difficulty
@onready var HIT_50 : float = 0.200 - 0.010 * difficulty
@onready var INPUT_DROP_TIME : float = 0.280 - 0.014 * difficulty

func _ready():
	pass
	
	#print(notes)

func calculate_hit_window():
	HIT_300 = 0.080 - 0.004 * difficulty
	HIT_100 = 0.160 - 0.008 * difficulty
	HIT_50 = 0.200 - 0.010 * difficulty
	INPUT_DROP_TIME = 0.280 - 0.014 * difficulty

func _input(event):
	if event is InputEventKey:
		inputs.append(event.duplicate())

func _process(delta):
	change_timer(delta)
	
	check_and_create_notes()
	
	call_deferred("_idle")

func _idle():
	inputs.clear()

func change_timer(delta : float):
	timer += delta

func check_and_create_notes():
	for i in notes.keys():
		if !i.is_valid_float():
			continue
		if float(i) < timer + 1:
			if notes[i].size() == 3:
				make_note(float(i), notes[i][0], notes[i][1], notes[i][2])
			if notes[i].size() == 4:
				make_note(float(i), notes[i][0], notes[i][1], notes[i][2], notes[i][3])
			notes.erase(i)

func make_note(hit_time : float, note_type : int, key : int, pallete : int, duration : int = 0):
	var new_note : Note
	match note_type:
		0:
			new_note = NoteNormal.new()
		1:
			new_note = NoteMine.new()
		2:
			new_note = NoteHold.new()
	
	new_note.key_number = key
	new_note.pallete = pallete
	new_note.hit_time = hit_time
	if note_type == 2:
		new_note.end_time = hit_time + duration
	new_note.botplay = botplay
	add_child(new_note)

func appearence_equation(hit_time : float):
	return 1 - abs(timer - hit_time) * (1 + float(approach) / 4)

func disappearecne_equation(pass_time : float):
	return (timer - pass_time) * 4

func calculate_accuracy():
	var acc_percent : float = float(overall_points) / float(point_amount * 30)
	accuracy_text(acc_percent)
	return acc_percent

func accuracy_text(acc_percent):
	var acc_number : float = round(acc_percent * 10000) / 100
	$accuracy.text = String.num(acc_number)

func note_pressed(points : int, pos : Vector2):
	overall_points += points
	point_amount += 1
	if points != 0:
		hit_note_amount += 1
	var texture : Texture
	match points:
		30:
			texture = preload("res://hit_300.png")
		10:
			texture = preload("res://hit_100.png")
		5:
			texture = preload("res://hit_50.png")
		0:
			texture = preload("res://miss.png")
	make_particle(texture, pos)
	calculate_accuracy()

func make_particle(texture : Texture, pos : Vector2):
	var new_part : Sprite2D = Particle.new()
	#, 0.1, 0.5, 0.5
	new_part.texture = texture
	new_part.position = pos
	new_part.start_interval = 0.1 - 0.05 * approach
	new_part.visible_interval = 0.5 - 0.025 * approach
	new_part.remove_interval = 0.5 - 0.025 * approach
	
	add_child(new_part)
