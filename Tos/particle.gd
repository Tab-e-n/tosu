class_name Particle
extends Sprite2D

@export var start_interval : float = 0.1
@export var visible_interval : float = 1
@export var remove_interval : float = 0.5

var timer : float = 0

func _ready():
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	
	var start_time : float = start_interval
	var visible_time : float = start_interval + visible_interval
	var remove_time : float = start_interval + visible_interval + remove_interval
	
	timer += delta
	
	if timer < start_time:
		modulate.a = timer / start_interval
	elif timer < visible_time:
		modulate.a = 1
	elif timer < remove_time:
		modulate.a = 1 - (timer - visible_time) / remove_interval
	else:
		queue_free()
	
