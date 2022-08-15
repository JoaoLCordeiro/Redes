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



int main(){
    
    char buffer_c[TAM_BUF];
    int soquete = ConexaoRawSocket("lo");


    

    while (1) {

        
        printf("Digite o comando\n");
        scanf("%s", buffer_c);

        msg_t mensagem;
        switch (compara_comando(buffer_c)) {

            case PUT:
                //envia_mensagem_put
                printf("Digite o nome do arquivo\n");
                scanf("%s", buffer_c);
                init_mensagem(&mensagem, strlen(buffer_c), 0, PUT, buffer_c);
                if (send(soquete, &mensagem, sizeof(msg_t), 0) < 0){
                    perror("send(): Error");
                }
                switch (recebe_mensagem_cliente_put(soquete, &mensagem)) {
                    case OK:
                        imprime_mensagem(&mensagem);
                        break;

                    default:
                        break;
                }
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
