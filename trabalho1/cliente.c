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
        scanf("%s", buffer_c);

        msg_t mensagem;
        switch (compara_comando(buffer_c)) {

            case PUT:
                trata_put_cliente (soquete);
                break;
            case GET:
                //envia_mensagem_get
                if (send(soquete, buffer_c, TAM_BUF, 0) < 0){
                    perror("send(): Error");
                }
                break;
            case MKDIR:
                //envia_mensagem_mkdir
                if (send(soquete, buffer_c, TAM_BUF, 0) < 0){
                    perror("send(): Error");
                }
                break;
            case LS:
                //envia_mensagem_ls
                if (send(soquete, buffer_c, TAM_BUF, 0) < 0){
                    perror("send(): Error");
                }
                break;
            case CD:
                //envia_mensagem_cd
                if (send(soquete, buffer_c, TAM_BUF, 0) < 0){
                    perror("send(): Error");
                }
                break;
            default:
                printf("Comando não existente\n");
                break;
            }

    }

    return 0;
}
