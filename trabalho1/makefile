CFLAGS = -Wall
OBJS = servidor.o ConexaoRawSocket.o

all: servidor

servidor: $(OBJS)
	gcc -o servidor $(CFLAGS) $(OBJS)

ConexaoRawSocket.o: ConexaoRawSocket.c
	gcc $(CFLAGS) -c ConexaoRawSocket.c

servidor.o: servidor.c
	gcc $(CFLAGS) -c servidor.c

clean:
	-rm -f *~ *.o