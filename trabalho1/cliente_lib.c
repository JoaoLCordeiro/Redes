#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

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

    init_mensagem(&mensagem, tamanho, 0, PUT, file_name);
    
    //enviar mensagem
    //Esperar resposta

}


int recebe_mensagem_cliente_put(int soquete, msg_t *mensagem){

    while (1) {
        if (recv(soquete, mensagem, sizeof(msg_t), 0) < 0) 
            perror("recv(): Error");
        
        // Verifica se o marcador de início e a paridade são os corretos
        if (mensagem->marc_inicio == MARC_INICIO) {
            if (testa_paridade(mensagem)) {

                // Verifica qual mensagem chegou do servidor
                if (mensagem->tipo == ERRO) 
                    return ERRO;
                else if (mensagem->tipo == NACK){
                    if (send(soquete, &mensagem, sizeof(msg_t), 0) < 0)
                        perror("send(): recebeu nack e tentou reenviar");
                }
                else {
                    return OK;
                }
            
            }
            else
                return NACK;
        }
        return NACK;
    }

}


// int put_client (int soquete, char *file_name) {
    
//     if (envia_nome_put (file_name) < 0){
//         //erro: server n respondeu positivamente
//         //return ??????
//     }
//     //passando daqui, o servidor ja deixou começar a escrever o arquivo
    
//     return 0;

//     FILE *arquivo;
//     char buffer[64];
    
//     arquivo = fopen (file_name, "r");
//     if (! arquivo) {
//         fprintf(stderr, "put_client() : Não foi possível abrir o arquivo %s\n", file_name);
//         return -1;
//     }

//     int tam = file_size(arquivo);
//     if (envia_tam_put (tam) < 0){
//         //erro: servern respondeu positivamente
//         //return ???????
//     }
//     //passando daqui, o servidor possui espaço para escrever o arquivo


//     int tamanho_lido = fread(buffer, sizeof(char), MAX_SIZE_MSG, arquivo);
//     while (tamanho_lido == 64) {
//         //fwrite(buffer, sizeof(char), MAX_SIZE_MSG, arquivo_saida);
//         tamanho_lido = fread(buffer, sizeof(char), MAX_SIZE_MSG, arquivo);
//     }
//     if (tamanho_lido > 0) {
//         //fwrite(buffer, sizeof(char), tamanho_lido, arquivo_saida);
//     }

// }