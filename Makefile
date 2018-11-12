run: linking
	./shell

linking: compile
	gcc bin/main.o bin/background.o bin/command.o bin/history.o bin/queue.o  -o shell

compile:
	gcc -c src/main.c -o bin/main.o
	gcc -c src/command.c -o bin/command.o
	gcc -c src/history.c -o bin/history.o
	gcc -c src/queue.c -o bin/queue.o
	gcc -c src/background.c -o bin/background.o
