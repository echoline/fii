CFLAGS=`pkg-config --cflags gtk+-3.0` -g
OBJS=main.o msgs.o menu.o

fii: $(OBJS)
	gcc -g -o fii $(OBJS) `pkg-config --libs gtk+-3.0`

install: fii
	cp fii /usr/local/bin

clean:
	rm -f *.o fii
