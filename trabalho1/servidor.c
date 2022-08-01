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
    
    int soquete = ConexaoRawSocket("lo");
    // printf ("soquete -> %d\n", soquete);

    char buffer_s[TAM_BUF];

    while (1) {
        
        if (recv(soquete, buffer_s, TAM_BUF, 0) < 0) {
            perror("recv(): Error");
        }

        printf("Chegou mensagem tal -> %s\n", buffer_s);
    }
    
    return 0;
}