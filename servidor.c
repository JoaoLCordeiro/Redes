#include <stdio.h>
#include <string.h>
#include "ConexaoRawSocket.h"

#define TAM_BUF 100

int main(){
    
    char buffer_s[TAM_BUF], buffer_c[TAM_BUF];

    scanf("%s", buffer_c);

    int soquete = ConexaoRawSocket("lo");
    printf ("soquete -> %d\n", soquete);

    if (send(soquete, &buffer_c, strlen(buffer_c), 0) < 0){
        perror("send(): Error");
    }

    if (recv(soquete, buffer_s, TAM_BUF, 0) < 0) {
        perror("recv(): Error");
    }

    printf("Servidor -> %s\n", buffer_s);

    return 0;
}