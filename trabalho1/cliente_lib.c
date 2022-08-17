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


void put_dados_cliente (int soquete){
    msg_t mensagem;

    char buffer_arq[TAM_BUF];
    //abre o arquivo
    while (/*AQUIVO NAO ACABOU*/){
        buffer_arq = /*le pedaco do arquivo*/

        init_mensagem(&mensagem, sizeof(buffer_arq), 0, DADOS, buffer_arq);

        bool ack = false;

        while (! ack){
            if (! manda_mensagem (soquete, &mensagem))
                perror("Erro ao enviar mensagem no put_dados");

            switch (recebe_retorno_put(soquete, &mensagem)) {
                //se for ack, quebra o laço interno e vai pro laço externo pegar mais dados
                case ACK:
                    ack = true;
                    break;

                //dá break e re-envia a msg quando volta o laço
                case NACK:
                    break;
            }
        }
    }

    //manda uma mensagem do tipo FIM
    init_mensagem(&mensagem, 0, 0, FIM, "");

    //considerando que o servidor responde um FIM com um ACK
    while (! ack){
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no put_dados");

        switch (recebe_retorno_put(soquete, &mensagem)) {
            //se for ack, acaba
            case ACK:
                return;
            //dá break e re-envia a msg quando volta o laço
            case NACK:
                break;
        }
    }
}

void put_tamanho_cliente (int soquete){
    msg_t mensagem;
    
    int tamanho = 10;
    //tamanho = tamanho_do_arquivo

    //tipo DESC pois acho que o TAMANHO conta como descrição
    init_mensagem(&mensagem, sizeof(int), 0, DESC, &tamanho);

    while (1){
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no put_tamanho");

        switch (recebe_retorno_put(soquete, &mensagem)) {
            //se for ok, continua
            case OK:
                put_dados_cliente(soquete);
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
    msg_t mensagem;
    char buffer_nome[TAM_BUF];

    //envia_mensagem_put
    printf("Digite o nome do arquivo\n");
    scanf("%s", buffer_c);
    init_mensagem(&mensagem, strlen(buffer_nome), 0, PUT, buffer_nome);

    while (1){
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_put_cliente");

        switch (recebe_retorno_put(soquete, &mensagem)) {
            //se for ok, continua
            case OK:
                put_tamanho_cliente(soquete);
                break;

            //se for erro, vai pegar outro comando
            case ERRO:
                printf("Erro no comando PUT: arquivo não permitido\n");
                return;
                break;

            //dá break e re-envia a msg quando volta o laço
            case NACK:
                break;
        }
    }

    return;
}

int recebe_retorno_put(int soquete, msg_t *mensagem){
    while (1) {
        if (! recebe_mensagem (int soquete, msg_t *mensagem)) 
            perror("Erro ao receber mensagem no recebe_retorno_put");
        
        // Verifica se o marcador de início e a paridade são os corretos
        if (mensagem->marc_inicio == MARC_INICIO) {
            //Testa a paridade
            if (testa_paridade(mensagem)) {

                // Verifica qual mensagem chegou do servidor
                if (mensagem->tipo == ERRO) 
                    return ERRO;
                
                //se for um NACK, reenvia a mensagem
                else if (mensagem->tipo == NACK){
                    //aqui nao damos return pro laço recomeçar e esperar mais uma resposta
                    if (! manda_mensagem (soquete, mensagem))
                        perror("Erro ao re-mandar mensagem no recebe_retorno_put");
                }
                else if (mensagem->tipo == ACK)
                    return ACK;
                else 
                    return OK;
            
            }
            else
            //retorna NACK para mensagens com erro no marcador ou na paridade
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