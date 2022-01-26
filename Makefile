CC = gcc
CFLAGS = -Wall -pedantic -std=c11

exec_files := *.o vm

vm: main.o
	$(CC) main.o -o vm
	echo "VM is ready."

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o
clean:
	rm -rf $(exec_files)
