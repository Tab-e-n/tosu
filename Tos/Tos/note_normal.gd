class_name NoteNormal
extends Note
# Basic note that the player has to hit


#var hit_300 : float = 0.080
#var hit_100 : float = 0.160
#var hit_50 : float = 0.200
#var input_drop_time : float = 0.280

#@export var key_number : int = 0
#@export var hit_time : float = 2
#@export var pallete : int = 0

#var key : String = "Q"
#var appearence : float = 0
#var disappearence : float = 0
#var pressed : bool = false

@onready var main : Label = Label.new()
#@onready var hit_circle : Line2D
@onready var border : Sprite2D = Sprite2D.new()
@onready var hit_circle : Sprite2D = Sprite2D.new()
@onready var inside : Sprite2D = Sprite2D.new()
#@onready var parent : Node = get_parent()

func _note_ready():
	#hit_circle = Line2D.new()
	#hit_circle.add_point(Vector2(-50, -50), 0)
	#hit_circle.add_point(Vector2(50, -50), 1)
	#hit_circle.add_point(Vector2(50, 50), 2)
	#hit_circle.add_point(Vector2(-50, 50), 3)
	#hit_circle.add_point(Vector2(-50, -50), 4)
	#hit_circle.position = Vector2(48, 48)
	#hit_circle.width = 4
	#add_child(hit_circle)
	
	inside.texture = preload("res://NormalInside.png")
	inside.position = Vector2(48, 48)
	inside.modulate = parent.note_palletes[pallete]
	inside.modulate.a = 0
	add_child(inside)
	
	border.texture = preload("res://NormalBorder.png")
	border.position = Vector2(48, 48)
	border.modulate.a = 0
	add_child(border)
	
	hit_circle.texture = preload("res://NormalHitCircle.png")
	hit_circle.position = Vector2(48, 48)
	hit_circle.modulate.a = 0
	hit_circle.scale = Vector2(2, 2)
	add_child(hit_circle)
	
	#main
	main.text = key
	main.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	main.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	main.theme = preload("res://default_theme.tres")
	main.clip_text = true
	main.size = Vector2(96, 96)
	main.modulate.a = 0
	add_child(main)
	
	#sprite = Sprite2D.new()
	#sprite.modulate.a = 0
	#sprite.texture = preload("res://icon.svg")
	#add_child(sprite)

func _start_interval(appearence : float):
	inside.modulate.a = appearence
	border.modulate.a = appearence
	main.modulate.a = appearence
	hit_circle.modulate.a = appearence
	hit_circle.scale = Vector2(2 - appearence, 2 - appearence)

func _end_interval(disappearence : float):
	inside.modulate.a = 1 - disappearence
	border.modulate.a = 1 - disappearence
	main.modulate.a = 1 - disappearence
	hit_circle.modulate.a = 1 - disappearence
	hit_circle.scale.x = disappearence
	hit_circle.scale.y = disappearence

func _hit_300():
	parent.note_pressed_normal(30, position + Vector2(48, 48))

func _hit_100():
	parent.note_pressed_normal(10, position + Vector2(48, 48))

func _hit_50():
	parent.note_pressed_normal(5, position + Vector2(48, 48))

func _hit_0():
	parent.note_pressed_normal(0, position + Vector2(48, 48))

func _miss():
	parent.note_pressed_normal(0, position + Vector2(48, 48))
