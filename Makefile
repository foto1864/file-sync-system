make:
	gcc -o manager src/manager.c
	gcc -o worker src/worker.c

run:
	./manager test-files/text.txt backup/test.txt -l logs/file-sync-logs.txt 

clean:
	- rm -rf backup
	- rm exec
	- rm worker