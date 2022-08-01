#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "geral.h"
#include "cliente_lib.h"


int file_size(FILE *arquivo) {

    fseek(arquivo, 0L, SEEK_END);
    unsigned int size = ftell(arquivo);
    fseek(arquivo, 0L, SEEK_SET);

    return size;
}


int envia_nome_put(char *file_name) {

    msg_t mensagem;
    int tamanho = strlen(file_name);

    init_mensagem(&mensagem, tamanho, PUT, file_name);
    
    //enviar mensagem
    //Esperar resposta

}


int put_client (int soquete, char *file_name) {
    
    if (envia_nome_put (file_name) < 0){
        //erro: server n respondeu positivamente
        //return ??????
    }
    //passando daqui, o servidor ja deixou começar a escrever o arquivo
    
    FILE *arquivo;
    char buffer[64];
    
    arquivo = fopen (file_name, "r");
    if (! arquivo) {
        fprintf(stderr, "put_client() : Não foi possível abrir o arquivo %s\n", file_name);
        return -1;
    }

    int tam = file_size(arquivo);
    if (envia_tam_put (tam) < 0){
        //erro: servern respondeu positivamente
        //return ???????
    }
    //passando daqui, o servidor possui espaço para escrever o arquivo


    int tamanho_lido = fread(buffer, sizeof(char), MAX_SIZE_MSG, arquivo);
    while (tamanho_lido == 64) {
        //fwrite(buffer, sizeof(char), MAX_SIZE_MSG, arquivo_saida);
        tamanho_lido = fread(buffer, sizeof(char), MAX_SIZE_MSG, arquivo);
    }
    if (tamanho_lido > 0) {
        //fwrite(buffer, sizeof(char), tamanho_lido, arquivo_saida);
    }

}