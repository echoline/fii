CFLAGS=`pkg-config --cflags gtk+-3.0` -g

fii: main.o msgs.o
	gcc -g -o fii `pkg-config --libs gtk+-3.0` main.o msgs.o

clean:
	rm -f *.o fii
