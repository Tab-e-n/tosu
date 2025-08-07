class_name NoteHoldEnd
extends Note
# Basic note that the player has to hit

@onready var hit_circle : Sprite2D = Sprite2D.new()
@onready var parent : Node = get_parent()


func _note_ready():
	should_input_be_pressed = false
	should_delete_self = false
	
	hit_circle.texture = preload("res://HoldHitCircleEnd.png")
	hit_circle.position = Vector2(48, 48)
	hit_circle.modulate.a = 0
	hit_circle.scale = Vector2(2, 2)
	add_child(hit_circle)
	
	position = Vector2(0, 0)


func _start_interval(appearence : float):
	hit_circle.modulate.a = appearence
	hit_circle.scale = Vector2(2 - appearence, 2 - appearence)


func _middle_interval():
	hit_circle.modulate.a = 1
	hit_circle.scale = Vector2(1, 1)


func _end_interval(disappearence : float):
	hit_circle.modulate.a = 1 - disappearence
	hit_circle.scale = Vector2(1 + disappearence, 1 + disappearence)


func _released_300():
	parent.note_pressed(30, true)


func _released_100():
	parent.note_pressed(10, true)


func _released_50():
	parent.note_pressed(5, true)


func _released_0():
	parent.note_pressed(0, true)


func _miss():
	parent.note_pressed(0, true)

