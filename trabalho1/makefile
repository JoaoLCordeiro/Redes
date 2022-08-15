CFLAGS = -Wall
OBJSS = servidor.o servidor_lib.o ConexaoRawSocket.o geral.o
OBJSC = cliente.o cliente_lib.o ConexaoRawSocket.o geral.o

all: servidor cliente
server: servidor
client: cliente

cliente: $(OBJSC)
	gcc -o cliente $(CFLAGS) $(OBJSC)

servidor: $(OBJSS)
	gcc -o servidor $(CFLAGS) $(OBJSS)

ConexaoRawSocket.o: ConexaoRawSocket.c
	gcc $(CFLAGS) -c ConexaoRawSocket.c

servidor.o: servidor.c
	gcc $(CFLAGS) -c servidor.c

servidor_lib.o: servidor_lib.c
	gcc $(CFLAGS) -c servidor_lib.c

cliente_lib.o: cliente_lib.c
	gcc $(CFLAGS) -c cliente_lib.c

cliente.o: cliente.c
	gcc $(CFLAGS) -c cliente.c

geral.o: geral.c
	gcc $(CFLAGS) -c geral.c

clean:
	-rm -f *~ *.o

purge: clean
	-rm servidor cliente