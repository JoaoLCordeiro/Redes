#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

#include "geral.h"
#include "servidor_lib.h"


void put_dados_server(int soquete, msg_t *mensagem, char *nome_arq) {

    printf("put_dados_sevidor\n");

    char aux[128];
    strcpy(aux, "./destino/");
    strcat(aux, nome_arq);

    FILE * arq = abre_arquivo(aux, "w+");
    if (fwrite(mensagem->dados, sizeof(char), mensagem->size_msg-1, arq) != mensagem->size_msg-1){
        perror("put_server(): Escreveu tamanho errado no servidor");
        return;
    }

    msg_t mensagem_ack;
    
    
    while (1){
        init_mensagem(&mensagem_ack, 0, sequencia_global, ACK, "");
        if (! manda_mensagem (soquete, &mensagem_ack))
            perror("Erro ao enviar mensagem no trata_put_servidor");

        switch (recebe_retorno(soquete, mensagem)) {
            //se for TAM, continua
            case DADOS:
                if (fwrite(mensagem->dados, sizeof(char), mensagem->size_msg-1, arq) != mensagem->size_msg-1){
                    perror("put_server(): Escrever tamanho errado no servidor");
                    return;
                }           
                break;

            case FIM:
                fclose(arq);
                return;
                break;
            //dá break e re-envia o ok
            case NACK:

                break;

            default:
                break;
        }
    }
    

}


void put_tamanho_server(int soquete, char *nome_arq){
    
    printf("put_tamanho_sevidor\n");
    msg_t mensagem;
    
    init_mensagem(&mensagem, 0, sequencia_global, OK, "");
    
    while (1){
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_put_servidor");

        switch (recebe_retorno(soquete, &mensagem)) {
            //se for TAM, continua
            case DADOS:
                put_dados_server(soquete, &mensagem, nome_arq);
                return;
                break;

            //dá break e re-envia o ok
            case NACK:
                break;

            default:
                break;
        }
    }

    return;

}


void trata_put_servidor(int soquete, msg_t* msg_put_inicial){

    printf("trata_put_sevidor\n");

    msg_t mensagem;
    char nome_arquivo[BUFFER_IMENSO];

    strcpy(nome_arquivo, msg_put_inicial->dados);

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
                put_tamanho_server(soquete, nome_arquivo);
                return;
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
        if (mensagem->marc_inicio == MARC_INICIO) {
            if (testa_paridade(mensagem))
                return mensagem->tipo;
            else
                return NACK;
        }
        return NACK;
    }

}