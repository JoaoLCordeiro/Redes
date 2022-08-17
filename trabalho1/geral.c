#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

#include "geral.h"

int manda_mensagem (int soquete, msg_t *mensagem){
    if (send(soquete, &mensagem, sizeof(msg_t), 0) < 0)
        return 0;
    return 1;
}

int recebe_mensagem (int soquete, msg_t *mensagem){
    if (recv(soquete, mensagem, sizeof(msg_t), 0) < 0) 
        return 0;
    return 1;
}

char calcula_paridade (int tamanho, char * dados){
    
    char aux = dados[0];
    for (int i = 1; i < tamanho; i++)
        aux = aux ^ dados[i];
    
    return aux;
}

int testa_paridade(msg_t*mensagem) {

    int paridade = calcula_paridade(mensagem->size_msg, mensagem->dados);
    if (paridade == mensagem->paridade)
        return 1;
    
    return 0;

}

int compara_comando(char *comando){

    if (! strcmp(comando, "put"))
        return PUT;
    else if (! strcmp(comando, "get"))
        return GET;
    else if (! strcmp(comando, "mkdir"))
        return MKDIR;
    else if (! strcmp(comando, "ls"))
        return LS;
    else if (! strcmp(comando, "cd"))
        return CD;
    
    return 0;


}


void init_dados(int tamanho, char *buffer_dados, char *dados){

    memset(buffer_dados, 0, TAM_BUFFER_DADOS);
    memcpy(buffer_dados, dados, tamanho);

}


void imprime_mensagem(msg_t *mensagem) {

    printf("Marcador de Início -> %d\n", mensagem->marc_inicio);
    printf("Tamanho -> %d\n", mensagem->size_msg);
    printf("Sequencia -> %d\n", mensagem->sequencia);
    printf("Tipo -> %d\n", mensagem->tipo);
    printf("Mensagem ->%s\n", mensagem->dados);
    printf("Paridade %d\n", (int)mensagem->paridade);

}


void init_mensagem(msg_t *mensagem, int tamanho, int sequencia, int tipo_mensagem, char *dados) {
        
    mensagem->marc_inicio = MARC_INICIO;
    mensagem->size_msg = tamanho;
    mensagem->sequencia = sequencia;
    mensagem->tipo = tipo_mensagem;
    init_dados(tamanho, mensagem->dados, dados);
    mensagem->paridade = (int)calcula_paridade(mensagem->size_msg, mensagem->dados);
}

void envia_ok(int soquete) {

    msg_t aux;
    init_mensagem(&aux, 0, 0, OK, "");
    if (send(soquete, &aux, sizeof(msg_t), 0) < 0){
        perror("send(): envia_ok");
        return;
    }

}