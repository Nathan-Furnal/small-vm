CC = gcc
CFLAGS = -Wall -pedantic -std=c11

exec_files := *.o vm

vm: utils.o vm.o main.o 
	$(CC) $(CFLAGS) main.o vm.o utils.o -o vm
	echo "VM is ready."

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

vm.o: vm.c
	$(CC) $(CFLAGS) -c vm.c -o vm.o

utils.o: utils.c
	$(CC) $(CFLAGS) -c utils.c -o utils.o
clean:
	rm -rf $(exec_files)
