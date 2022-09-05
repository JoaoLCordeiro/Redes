#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

#include "ConexaoRawSocket.h"
#include "servidor_lib.h"
#include "geral.h"

#define TAM_BUF 100

int sequencia_global = 1;

int main(){
    
	//int soquete = ConexaoRawSocket("lo");
    int soquete = ConexaoRawSocket("eno1");
    // printf ("soquete -> %d\n", soquete);

    msg_t mensagem;
    while (1) {
          
        
        switch (recebe_mensagem_server(soquete, &mensagem))
        {
        case PUT:
            trata_put_servidor(soquete, &mensagem);
            break;
        
        case GET:
            trata_get_servidor(soquete, &mensagem);
            break;
        case MKDIR:
            trata_mkdir_servidor(soquete, &mensagem);
            break;

        case LS:
            trata_ls_servidor(soquete, &mensagem);
            break;
        case CD:
            trata_cd_servidor(soquete, &mensagem);
            break;
        case NACK:
            //envia_nack()
            
            break;
        }
        
        //imprime_mensagem(&mensagem);

    }
    
    return 0;
}