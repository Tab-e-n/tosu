extends Node2D

@export var difficulty : int = 0
@export var approach : int = 0

var timer : float = 0
var inputs : Array = []

var overall_points : int = 0
var point_amount : int = 0
var hit_note_amount : int = 0

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
const note_palletes : Array = [
	Color(0.9, 0, 0, 1),
]

var notes : Dictionary = {
	#hit_time : [note_type, key_type, pallete],
	2 : [0, 0, 0],
	3 : [0, 1, 0],
	3.4 : [0, 2, 0],
	3.8 : [0, 3, 0],
	4.5 : [1, 11, 0]
	
}

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func _input(event):
	if event is InputEventKey:
		inputs.append(event.duplicate())

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	timer += delta
	#print("main ", timer)
	
	for i in notes.keys():
		if i < timer + 1:
			make_note(i, notes[i][0], notes[i][1], notes[i][2])
			notes.erase(i)
	
	call_deferred("_idle")

func _idle():
	inputs.clear()

func make_note(hit_time : float, note_type : int, key : int, pallete : int):
	var new_note : Note
	match note_type:
		0:
			new_note = NoteNormal.new()
		1:
			new_note = NoteMine.new()
	
	new_note.key_number = key
	new_note.hit_time = hit_time
	new_note.pallete = pallete
	add_child(new_note)

func calculate_accuracy():
	var acc_percent : float = float(overall_points) / float(point_amount * 30)
	accuracy_text(acc_percent)
	return acc_percent

func accuracy_text(acc_percent):
	var acc_number : float = round(acc_percent * 10000) / 100
	$accuracy.text = String.num(acc_number)
	
func note_pressed_normal(points : int, pos : Vector2):
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
