class_name SquareSpinner
extends Polygon2D

@export var time : float = 0

func _ready():
	set_polygon([Vector2(48, 48), Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), Vector2(0, 0), Vector2(0, 0)])
	

func _process(_delta):
	#time += _delta
	#if time > 1:
		#time -= 1
	visible = time != 0
	if time == 1:
		polygon[2] = Vector2(96, 0)
		polygon[3] = Vector2(96, 96)
		polygon[4] = Vector2(0, 96)
		polygon[5] = Vector2(0, 0)
		
	elif time > 0.75:
		polygon[2] = Vector2(96, 0)
		polygon[3] = Vector2(96, 96)
		polygon[4] = Vector2(0, 96)
		
		var pos = Vector2(0, 96 - 96 * ((time - 0.75) / 0.25))
		polygon[5] = pos
	elif time > 0.5:
		polygon[2] = Vector2(96, 0)
		polygon[3] = Vector2(96, 96)
		
		var pos = Vector2(96 - 96 * ((time - 0.5) / 0.25), 96)
		polygon[4] = pos
		polygon[5] = pos
	elif time > 0.25:
		polygon[2] = Vector2(96, 0)
		
		var pos = Vector2(96, 96 * ((time - 0.25) / 0.25))
		polygon[3] = pos
		polygon[4] = pos
		polygon[5] = pos
	elif time > 0:
		var pos = Vector2(96 * (time / 0.25), 0)
		polygon[2] = pos
		polygon[3] = pos
		polygon[4] = pos
		polygon[5] = pos
