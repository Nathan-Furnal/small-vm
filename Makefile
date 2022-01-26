main.o: main.c
	cc -c main.c -o main.o
	echo "Build complete!"
clean:
	rm -f *.o
