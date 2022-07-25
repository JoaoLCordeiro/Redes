#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geral.h"

char * init_dados(int tamanho, char *dados){

    char *aux;

    //Aloca espaço para os dados e testa se foi alocado
    aux = malloc(sizeof(char)*(tamanho + 1));
    if (! aux){
        fprintf(stderr, "init_dados(): Não foi possível alocar memória para os dados\n");
        return NULL;
    }

    //Copia os dados
    strcpy(aux, dados);

    return aux;
}

void init_mensagem(msg_t *mensagem, int tamanho, int tipo_mensagem, char *dados) {

    mensagem->marc_inicio = MARC_INICIO;
    mensagem->size_msg = tamanho;
    mensagem->sequencia = sequencia;
    mensagem->tipo = tipo_mensagem;
    mensagem->dados = init_dados(tamanho, dados);
    if (! mensagem->dados) {
        fprintf(stderr, "init_mensagem() : Não foi possível alocar espaço para os dados\n");
        return;
    }
    mensagem->paridade = calcula_paridade(mensagem->size_msg, mensagem->dados);

}