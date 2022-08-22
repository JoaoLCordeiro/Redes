#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

#include "geral.h"
#include "servidor_lib.h"

void trata_put_servidor(int soquete, msg_t* msg_put_inicial){

    msg_t mensagem;

    //verifica permissão do diretório
    if (tem_permissao() < 0){
        perror("Você não tem permissão de escrita neste diretório\n");
        //envia_erro();
        return;
    }
    
    
    //cria um ok
    //imprime_mensagem(msg_put_inicial);
    init_mensagem(&mensagem, 0, sequencia_global, OK, "");
    
    while (1){
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_put_servidor");

        switch (recebe_retorno(soquete, &mensagem)) {
            //se for TAM, continua
            case DESC:
                printf("chegou\n");
                //put_tamanho_server(soquete);
                break;

            //dá break e re-envia o ok
            case NACK:
                break;

            default:
                printf("sincronismo\n");
                break;
        }
    }

    return;
}

int recebe_mensagem_server(int soquete, msg_t *mensagem) {

    while (1) {
        recebe_mensagem(soquete, mensagem);
        imprime_mensagem(mensagem);
        if (mensagem->marc_inicio == MARC_INICIO) {
            if (testa_paridade(mensagem))
                return mensagem->tipo;
            else
                return NACK;
        }
        return NACK;
    }

}