extends Control

var song_name : String
var chart_name : String

var notes : Dictionary = {
	#hit_time : [note_type, key_type, pallete], duration],
#	"1" : [1, 10, 0],
#	"1.0001" : [1, 11, 0],
#	"2" : [0, 0, 0],
#	"3" : [0, 1, 0],
#	"3.4" : [0, 2, 0],
#	"3.8" : [0, 3, 0],
#	"4.5" : [1, 11, 0],
#	"2" : [2, 16, 0, -3],
	"bpm" : 240,
	"note_palletes" : [Color(0.9, 0, 0, 1)],
	"difficulty" : 0,
	"approach" : 10,
}

# ""

func _ready():
	pass
	#save_chart("res://chart.ch", notes)
	#notes = load_chart("res://chart.ch").duplicate()

#func _process(delta):
	#pass

func save_chart(path : String, chart : Dictionary):
	var file = FileAccess.open(path, FileAccess.WRITE)
	
	file.store_line(JSON.stringify(chart))
	
	file.close()
	
	#print(chart)

func load_chart(path : String):
	var file = FileAccess.open(path, FileAccess.READ)
	
	if !file.file_exists(path):
		return {}
	
	var json : JSON = JSON.new()
	var chart : Dictionary = {}
	
	while !file.eof_reached():
		json.parse(file.get_line())
		chart.merge(json.data)
	
	file.close()
	
	#print(chart)
	
	return chart
