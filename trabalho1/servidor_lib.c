#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

#include "geral.h"
#include "servidor_lib.h"

void trata_put_servidor(int soquete, msg_t* mensagem){

    msg_t mensagem;

    //if (! tem_permissao())
        //envia_erro()
        //return;

    //cria um ok
    init_mensagem(&mensagem, 0, 0, OK, "");
   
    while (1){
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_put_servidor");

        switch (recebe_retorno_put(soquete, &mensagem)) {
            //se for TAM, continua
            case TAM:
                put_tamanho_server(soquete);
                break;

            //dá break e re-envia o ok
            case NACK:
                break;
        }
    }

    return;
}

int recebe_mensagem_server(int soquete, msg_t *mensagem) {

    while (1) {
        if (recv(soquete, mensagem, sizeof(msg_t), 0) < 0) 
            perror("recv(): Error");
        
        if (mensagem->marc_inicio == MARC_INICIO) {
            if (testa_paridade(mensagem))
                return mensagem->tipo;
            else
                return NACK;
        }
        return NACK;
    }

}