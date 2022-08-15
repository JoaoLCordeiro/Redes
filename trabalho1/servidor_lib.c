#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

#include "geral.h"


void trata_put_servidor(int soquete, msg_t* mensagem){

    //if (! tem_permissao())
        //envia_erro()
        //return;

    envia_ok(soquete);
    //recebe_mensagem_servidor_put()

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