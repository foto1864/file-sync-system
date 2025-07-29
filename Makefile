make:
	gcc -o main main.c
	gcc -o worker worker.c
	- rm backup.txt

run:
	./main text.txt backup.txt