class_name NoteMine
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
@onready var border : Sprite2D = Sprite2D.new()
@onready var inside : Sprite2D = Sprite2D.new()

func _note_ready():	
	inside.texture = preload("res://MineInside.png")
	inside.position = Vector2(48, 48)
	inside.modulate = parent.note_palletes[pallete]
	inside.modulate.a = 0
	add_child(inside)
	
	border.texture = preload("res://MineBorder.png")
	border.position = Vector2(48, 48)
	border.modulate = parent.note_palletes[pallete]
	border.modulate.a = 0
	add_child(border)
	
	#main
	main.text = key
	main.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
	main.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	main.theme = preload("res://default_theme.tres")
	main.add_theme_color_override("font_color", parent.note_palletes[pallete])
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

func _end_interval(disappearence : float):
	inside.modulate.a = 1 - disappearence
	border.modulate.a = 1 - disappearence
	main.modulate.a = 1 - disappearence

func _hit():
	parent.note_pressed_normal(0, position + Vector2(48, 48))
