#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

#include "geral.h"
#include "cliente_lib.h"


/******************************************PUT**************************************************/
void put_dados_cliente (int soquete, FILE * arq){

    int contador = 1;
    printf("put_dados_cliente\n");

    msg_t mensagem;

    char buffer_arq[TAM_BUFFER_DADOS];
    int bytes_lidos = fread(buffer_arq, sizeof(char), TAM_BUFFER_DADOS - 1, arq);
    
    while (bytes_lidos != 0){

        contador++;
        init_mensagem(&mensagem, bytes_lidos, sequencia_global, DADOS, buffer_arq);
        int ack = 0;
        while (! ack){
            if (! manda_mensagem (soquete, &mensagem))
                perror("Erro ao enviar mensagem no put_dados");

            switch (recebe_retorno(soquete, &mensagem)) {
                
                //se for ack, quebra o laço interno e vai pro laço externo pegar mais dados
                case ACK:
                    ack = 1;
                    break;

                //dá break e re-envia a msg quando volta o laço
                case NACK:
                    break;
            }
        }
        memset(buffer_arq, 0, TAM_BUFFER_DADOS);
        bytes_lidos = fread(buffer_arq, sizeof(char), TAM_BUFFER_DADOS - 1, arq);
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
                printf("put_dados_cliente: recebeu um ack do server, retornando...\n");
                printf("Contador -> %d\n", contador);
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

    FILE *arq = abre_arquivo(nome_arquivo, "rb");


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
                printf("put_tamanho_cliente: retornando...\n");
                return;
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
                printf("trata_put_cliente: retornando...\n");
                return;
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
/**************************************FIM PUT**************************************************/

/***************************************GET*****************************************************/
void get_dados_cliente(int soquete, msg_t* mensagem, char *nome_arquivo){


    printf("get_dados_cliente\n");

    char aux[128];
    //isso serve para criar outro arquivo, APENAS PARA TESTES
    strcpy(aux, "./destino_cliente/");
    strcat(aux, nome_arquivo);

    FILE * arq = abre_arquivo(aux, "wb+");
    if (fwrite(mensagem->dados, sizeof(char), mensagem->size_msg, arq) != mensagem->size_msg){
        perror("get_dados_cliente(): Escreveu tamanho errado no cliente");
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


void get_tamanho_cliente(int soquete, char *nome_arquivo, msg_t *msg_tam_server){


    //REMOVER COMENTARIO
    //aqui, o cliente deveria testar algo com o tamanho do arquivo, mas nao fizemos

    printf("get_tamanho_cliente\n");

    //cria um ok
    msg_t mensagem;
    
    while (1){
        //cria um ok
        init_mensagem(&mensagem, 0, sequencia_global, OK, "");

        //manda um ok
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_put_servidor");

        //Essa mensagem possui os primeiros bytes da mensagem
        switch (recebe_retorno(soquete, &mensagem)) {

            //recebeu os primeiros bytes do arquivo do servidor [get_dados_server]
            case DADOS:
                get_dados_cliente(soquete, &mensagem, nome_arquivo);

                //voltou da dados_server, agora dai dessa funcao
                return;
                break;

            case ERRO:
                return;
                break;
            //dá break e re-envia o ok
            case NACK:
                break;
        }
    }

    return;


}


void trata_get_cliente(int soquete){


    printf("trata_get_cliente\n");

    msg_t mensagem;
    char buffer_nome[TAM_BUFFER_DADOS];

    //envia_mensagem_put
    printf("Digite o nome do arquivo\n");
    scanf("%s", buffer_nome);

    init_mensagem(&mensagem, strlen(buffer_nome), sequencia_global, GET, buffer_nome);

    while (1){
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_put_cliente");

        /*Recebe uma mensagem com o tamanho do arquivo*/
        switch (recebe_retorno(soquete, &mensagem)) {
            
            //se for ok, continua
            case DESC:
                get_tamanho_cliente(soquete, buffer_nome, &mensagem);
                printf("trata_GET_cliente: retornando...\n");
                return;
                break;

            //se for erro, vai pegar outro comando
            case ERRO:
                printf("Erro no comando GET: arquivo não permitido\n");
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
/*****************************************FIM GET**********************************************/

/*****************************************MKDIR************************************************/
void trata_mkdir_cliente(int soquete){
    
    printf("trata_mkdir_cliente\n");

    msg_t mensagem;
    char buffer_nome[TAM_BUFFER_DADOS];

    //envia_mensagem_put
    printf("Digite o nome do diretório que você quer criar no servidor\n");
    scanf("%s", buffer_nome);

    init_mensagem(&mensagem, strlen(buffer_nome), sequencia_global, MKDIR, buffer_nome);

    while (1){
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_mkdir_cliente");

        /*Recebe uma mensagem com o tamanho do arquivo*/
        switch (recebe_retorno(soquete, &mensagem)) {
            
            //se for ok, continua
            case OK:
                manda_ack(soquete);
                printf("Diretório criado com sucesso\n");
                return;
                break;

            //se for erro, vai pegar outro comando
            case ERRO:
                manda_ack(soquete);
                printf("O diretório não foi criado\n");
                return;
                break;

            //dá break e re-envia a msg quando volta o laço
            case NACK:
                //manda_nack(soquete); 
                //manda mensagem novamente
                break;
        }
    }

    return;
}
/***************************************FIM MKDIR**********************************************/

/******************************************LS**************************************************/
void trata_ls_cliente(int soquete, char * buffer_c){

    printf("trata_ls_cliente\n");
    msg_t mensagem_ls;


    init_mensagem(&mensagem_ls, strlen(buffer_c), sequencia_global, LS, buffer_c);
    while (1){
        if (! manda_mensagem (soquete, &mensagem_ls))
            perror("Erro ao enviar mensagem no trata_ls_cliente");

        /*Recebe uma mensagem com o tamanho do arquivo*/
        switch (recebe_retorno(soquete, &mensagem_ls)) {
            
            //se for NA_TELA, printa na tela, manda ack e continua
            case NA_TELA:
                printf("%s", mensagem_ls.dados);
                init_mensagem(&mensagem_ls, 0, sequencia_global, ACK, "");
                break;

            //se for erro, avisa que deu erro e termina
            case ERRO:
                manda_ack(soquete);
                printf("O diretório não possui permissão de leitura\n");
                return;
                break;

            case NACK:
                //manda_nack(soquete); 
                //manda mensagem novamente
                break;

            // Se for fim, acabou.
            case FIM:
                manda_ack(soquete);
                return;
                break;
        }
    }

    return;

}
/***************************************FIM LS*************************************************/

/******************************************CD**************************************************/
void trata_cd_cliente(int soquete){

    printf("trata_cd_cliente\n");
    msg_t mensagem_cd;

    char caminho_diretorio[BUFFER_IMENSO];
    printf("Digite o diretório que você deseja ir\n");

    scanf("%65535[^\n]", caminho_diretorio);
    getchar();


    init_mensagem(&mensagem_cd, strlen(caminho_diretorio), sequencia_global, CD, caminho_diretorio);
    while (1){
        if (! manda_mensagem (soquete, &mensagem_cd))
            perror("Erro ao enviar mensagem no trata_cd_cliente");

        /*Recebe uma mensagem com o tamanho do arquivo*/
        switch (recebe_retorno(soquete, &mensagem_cd)) {
            
            //se for NA_TELA, printa na tela, manda ack e continua
            case OK:
                printf("CD dado com sucesso\n");
                manda_ack(soquete);
                return;
                break;

            //se for erro, avisa que deu erro e termina
            case ERRO:
                manda_ack(soquete);
                printf("Erro ao dar CD\n");
                return;
                break;

            case NACK:
                //manda_nack(soquete); 
                //manda mensagem novamente
                break;

            // Se for fim, acabou.
            case FIM:
                manda_ack(soquete);
                return;
                break;
        }
    }

    return;

}
/***************************************FIM cd*************************************************/