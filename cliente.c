#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

#include "ConexaoRawSocket.h"

#define TAM_BUF 100

int main(){
    
    char buffer_c[TAM_BUF];
    int soquete = ConexaoRawSocket("lo");


    while (1) {
        printf("Mandando mensagem\n");
        scanf("%s", buffer_c);

        // printf ("soquete -> %d\n", soquete);

        if (send(soquete, buffer_c, TAM_BUF, 0) < 0){
            perror("send(): Error");
        }
    }

    return 0;
}
