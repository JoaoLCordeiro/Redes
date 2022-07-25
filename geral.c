#include <stdio.h>
#include <stdlib.h>
#include "geral.h"

// void init_msg() {





// }

void put () {
    
    FILE *arquivo, *arquivo_saida;
    char buffer[64];
    
    arquivo = fopen ("/home/pingo/johann.cpp", "r");
    if (! arquivo) {
        perror("Erro:");
        exit(1);
    }

    int tamanho_lido = fread(buffer, sizeof(char), MAX_SIZE_MSG, arquivo);
    while (tamanho_lido == 64) {
        fwrite(buffer, sizeof(char), MAX_SIZE_MSG, arquivo_saida);
        tamanho_lido = fread(buffer, sizeof(char), MAX_SIZE_MSG, arquivo);
    }
    if (tamanho_lido > 0) {
        fwrite(buffer, sizeof(char), tamanho_lido, arquivo_saida);
    }

}

int main () {

    le_arquivo();

    return 0;
}