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
    //isso serve para criar outro arquivo, APENAS PARA TESTES
    strcpy(aux, "./destino/");
    strcat(aux, nome_arq);

    FILE * arq = abre_arquivo(aux, "wb+");
    if (fwrite(mensagem->dados, sizeof(char), mensagem->size_msg, arq) != mensagem->size_msg){
        perror("put_server(): Escreveu tamanho errado no servidor");
        return;
    }

    msg_t mensagem_ack;
    int conta_mensagens = 1;
    
    while (1){
        init_mensagem(&mensagem_ack, 0, sequencia_global, ACK, "");
        if (! manda_mensagem (soquete, &mensagem_ack))
            perror("Erro ao enviar mensagem no trata_put_servidor");

        switch (recebe_retorno(soquete, mensagem)) {
            //se for TAM, continua
            case DADOS:
                conta_mensagens++;
                if (fwrite(mensagem->dados, sizeof(char), mensagem->size_msg, arq) != mensagem->size_msg){
                    perror("put_server(): Escrever tamanho errado no servidor");
                    return;
                }           
                break;

            case FIM:
                init_mensagem(&mensagem_ack, 0, sequencia_global, ACK, "");
                manda_mensagem (soquete, &mensagem_ack);
                fclose(arq);
                printf("%d\n", conta_mensagens);
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
    
    //REMOVER COMENTARIO
    //aqui, o servidor deveria testar algo com o tamanho do arquivo, mas nao fizemos

    printf("put_tamanho_sevidor\n");

    //cria um ok
    msg_t mensagem;
    
    while (1){
        //cria um ok
        init_mensagem(&mensagem, 0, sequencia_global, OK, "");

        //manda um ok
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_put_servidor");

        switch (recebe_retorno(soquete, &mensagem)) {
            //se for DADOS, continua
            case DADOS:
                put_dados_server(soquete, &mensagem, nome_arq);

                //voltou da dados_server, agora dai dessa funcao
                return;
                break;

            //dá break e re-envia o ok
            case NACK:
                break;
        }
    }

    return;
}


void trata_put_servidor(int soquete, msg_t* msg_put_inicial){

    printf("trata_put_sevidor\n");

    msg_t mensagem;
    
    //coloca o nome do arquivo q está em msg_put_inicial no nome_arquivo
    char nome_arquivo[BUFFER_IMENSO];
    strcpy(nome_arquivo, msg_put_inicial->dados);

    //verifica permissão do diretório
    if (tem_permissao() < 0){
        perror("Você não tem permissão de escrita neste diretório\n");
        //envia_erro();
        return;
    }
    
    while (1){
        //cria um ok
        init_mensagem(&mensagem, 0, sequencia_global, OK, "");

        //manda um ok
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_put_servidor");

        switch (recebe_retorno(soquete, &mensagem)) {
            //se for o tamanho da mensagem, continua
            case DESC:
                put_tamanho_server(soquete, nome_arquivo);

                //voltou da função do tamanho, sai dessa funcao
                return;
                break;

            //dá break e re-envia o ok
            case NACK:
                break;
        }
    }

    //sai da funcao
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