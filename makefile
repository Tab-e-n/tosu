tosu.out : main.o notes.o chart.o
	gcc -o tosu.out main.o notes.o chart.o -lraylib

main.o : main.c notes.h chart.h
	gcc -c main.c -lraylib
notes.o : notes.c notes.h
	gcc -c notes.c -lraylib
chart.o : chart.c chart.h notes.h
	gcc -c chart.c -lraylib

clean :
	rm tosu.out main.o notes.o chart.o
