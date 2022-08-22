#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

#include "geral.h"
#include "cliente_lib.h"



void put_dados_cliente (int soquete, FILE * arq){

    printf("put_dados_cliente\n");

    msg_t mensagem;

    char buffer_arq[TAM_BUFFER_DADOS];
    
    while (fread(buffer_arq, sizeof(char), TAM_BUFFER_DADOS - 1, arq) != 0){
        printf("dentro do fread\n");
        //buffer_arq = /*le pedaco do arquivo*/

        init_mensagem(&mensagem, strlen(buffer_arq), sequencia_global, DADOS, buffer_arq);
        int ack = 0;
        while (! ack){
            printf("DENTRO DO WHILE ACK NO PUT DADOS CLIENTE\n");
            if (! manda_mensagem (soquete, &mensagem))
                perror("Erro ao enviar mensagem no put_dados");

            switch (recebe_retorno(soquete, &mensagem)) {
                
                //se for ack, quebra o laço interno e vai pro laço externo pegar mais dados
                case ACK:
                    printf("RECEBEU ACK\n");
                    ack = 1;
                    break;

                //dá break e re-envia a msg quando volta o laço
                case NACK:
                    break;
            }
        }
    }

    //manda uma mensagem do tipo FIM
    init_mensagem(&mensagem, 0, sequencia_global, FIM, "");

    //considerando que o servidor responde um FIM com um ACK
    while (1){
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no put_dados");

        switch (recebe_retorno(soquete, &mensagem)) {
            //se for ack, acaba
            case ACK:
                return;
            //dá break e re-envia a msg quando volta o laço
            case NACK:
                break;
        }
    }
}

void put_tamanho_cliente (int soquete, char *nome_arquivo){

    printf("put_tamanho_cliente\n");

    msg_t mensagem;

    FILE *arq = abre_arquivo(nome_arquivo, "r");


    int tamanho = tamanho_do_arquivo(arq);
    char tamanho_string[32];

    sprintf(tamanho_string, "%d", tamanho);

    //tipo DESC pois acho que o TAMANHO conta como descrição
    init_mensagem(&mensagem, strlen(tamanho_string), sequencia_global, DESC, tamanho_string);

    while (1){
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no put_tamanho");

        switch (recebe_retorno(soquete, &mensagem)) {
            //se for ok, continua
            case OK:
                put_dados_cliente(soquete, arq);
                break;

            //se for erro, vai pegar outro comando
            case ERRO:
                printf("Erro no comando PUT: sem espaço disponível\n");
                return;
                break;

            //dá break e re-envia a msg quando volta o laço
            case NACK:
                break;
        }
    }
}

void trata_put_cliente (int soquete){

    printf("trata_put_cliente\n");

    msg_t mensagem;
    char buffer_nome[TAM_BUFFER_DADOS];

    //envia_mensagem_put
    printf("Digite o nome do arquivo\n");
    scanf("%s", buffer_nome);

    if (check_permissao_existencia(buffer_nome) < 0) {
        perror("O Arquivo não existe ou você não possui permissão de leitura");
        return;
    }

    init_mensagem(&mensagem, strlen(buffer_nome), sequencia_global, PUT, buffer_nome);

    while (1){
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_put_cliente");

        switch (recebe_retorno(soquete, &mensagem)) {
            //se for ok, continua
            case OK:
                put_tamanho_cliente(soquete, buffer_nome);
                break;

            //se for erro, vai pegar outro comando
            case ERRO:
                printf("Erro no comando PUT: arquivo não permitido\n");
                return;
                break;

            //dá break e re-envia a msg quando volta o laço
            case NACK:
                printf("vasco");
                break;
        }
    }

    return;
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