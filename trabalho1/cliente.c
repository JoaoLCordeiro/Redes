#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

#include "ConexaoRawSocket.h"
#include "cliente_lib.h"
#include "geral.h"

#define TAM_BUF 100

int sequencia_global = 1;


int main(){
    
    char buffer_c[TAM_BUF];
    int soquete = ConexaoRawSocket("lo");


    

    while (1) {

        
        printf("Digite o comando\n");
        scanf("%63[^\n]", buffer_c);
        getchar();


        switch (compara_comando(buffer_c)) {

            case PUT:
                trata_put_cliente(soquete);
                break;
            case GET:
                trata_get_cliente(soquete);
                break; 
            case MKDIR:
                trata_mkdir_cliente(soquete);
                break;
            case LS:
                trata_ls_cliente(soquete, buffer_c);
                break;
            case CD:
                trata_cd_cliente(soquete);
                break;
            default:
                printf("Comando não existente\n");
                break;
            }

        memset(buffer_c, 0, TAM_BUF);
    }

    return 0;
}
