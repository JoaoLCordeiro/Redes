#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

#include "geral.h"

int manda_mensagem (int soquete, msg_t *mensagem){
    if (send(soquete, mensagem, sizeof(msg_t), 0) < 0)
        return 0;
    
    //print do manda mensagem
    //printf("Manda mensagem:\n");
   // imprime_mensagem(mensagem);
    //printf("\n");

    if (sequencia_global >= 15)
            sequencia_global = 1;
        else
            sequencia_global++;
    return 1;
}

int recebe_mensagem (int soquete, msg_t *mensagem){
    while (1){
        if (recv(soquete, mensagem, sizeof(msg_t), 0) < 0)
            return 0;
    
        if (mensagem->sequencia != sequencia_global) {
            continue;
        }

        //print do recebe mensagem
        //printf("Recebe mensagem:\n");
        //imprime_mensagem(mensagem);
        //printf("\n");

        if (sequencia_global >= 15)
            sequencia_global = 1;
        else
            sequencia_global++;
        return 1;
    }
}

char calcula_paridade (int tamanho, char * dados){
    
    // Faz um XOR dos bits para cada byte da mensagem.
    unsigned char aux = dados[0];
    for (int i = 1; i < tamanho; i++)
        aux = aux ^ dados[i];
    
    return aux;
}

int testa_paridade(msg_t*mensagem) {

    // Calcula a paridade da mensagem que chegou
    int paridade = (int)calcula_paridade(mensagem->size_msg, mensagem->dados);

    // Testa a paridade calculada com a paridade da mensagem que chegou
    // Se ela é igual ou o seu complemente de dois ela chegou certo (teoricamente)
    if (mensagem->paridade - paridade == 256 || paridade == mensagem->paridade)
        return 1;
    printf("PARIDADE CALCULADA %d ----- PARIDADE DA MENSAGEM %d\n", paridade, mensagem->paridade);    
    return 0;

}

int compara_comando(char *comando){

    // Testa o comando que o usuário digitou
    if (! strcmp(comando, "put"))
        return PUT;
    else if (! strcmp(comando, "get"))
        return GET;
    else if (! strcmp(comando, "mkdir"))
        return MKDIR;
    else if (! strcmp(comando, "ls") || ! strcmp(comando, "ls -a") || ! strcmp(comando, "ls -l"))
        return LS;
    else if (! strcmp(comando, "cd"))
        return CD;
    
    return 0;


}


void init_dados(int tamanho, char *buffer_dados, char *dados){

    //Limpa o buffer e copia os dados para o buffer de dados
    memset(buffer_dados, 0, TAM_BUFFER_DADOS);
    memcpy(buffer_dados, dados, tamanho);

}


void imprime_mensagem(msg_t *mensagem) {

    printf("Marcador de Início -> %d\n", mensagem->marc_inicio);
    printf("Tamanho -> %d\n", mensagem->size_msg);
    printf("Sequencia -> %d\n", mensagem->sequencia);
    printf("Tipo -> %d\n", mensagem->tipo);
    printf("Mensagem ->%s\n", mensagem->dados);
    printf("Paridade -> %d\n", (int)mensagem->paridade);
    printf("Sequência global -> %d\n", sequencia_global);

}


void init_mensagem(msg_t *mensagem, int tamanho, int sequencia, int tipo_mensagem, char *dados) {
        
    // Inicializa os campos da estrutura mensagem
    mensagem->marc_inicio = MARC_INICIO;
    mensagem->size_msg = tamanho;
    mensagem->sequencia = sequencia;
    mensagem->tipo = tipo_mensagem;
    init_dados(tamanho, mensagem->dados, dados);
    mensagem->paridade = (unsigned int)calcula_paridade(mensagem->size_msg, mensagem->dados);
}


int recebe_retorno(int soquete, msg_t *mensagem){
    
    while (1) {
        // Recebe uma mensagem
        if (! recebe_mensagem (soquete, mensagem)) 
            perror("Erro ao receber mensagem no recebe_retorno");
        
        // Verifica se o marcador de início e a paridade são os corretos
        if (mensagem->marc_inicio == MARC_INICIO) {
            //Testa a paridade
            if (testa_paridade(mensagem)) {

                //se for um NACK, reenvia a mensagem
                if (mensagem->tipo == NACK){
                    //aqui nao damos return pro laço recomeçar e esperar mais uma resposta
                    if (! manda_mensagem (soquete, mensagem))
                        perror("Erro ao re-mandar mensagem no recebe_retorno_put");
                }

                // Senão retorna o tipo    
                else {
                    return mensagem->tipo;
                }
            
            }
            else {
            //retorna NACK para mensagens com erro no marcador ou na paridade
                return NACK;
            }    
        }
        printf("MARCADOR DE INÍCIO DEU RUIM\n");
        return NACK;
    }

}

int tem_permissao() {

    char diretorio[BUFFER_IMENSO];

    strcpy(diretorio, "./");

    return access(diretorio, W_OK);
 
}

int check_permissao_existencia(char *nome_arquivo) {

    char arquivo[BUFFER_IMENSO];

    //coloca a string que representa o arquivo em um ./
    strcpy(arquivo, "./");
    strcat(arquivo, nome_arquivo);

    //testa se o arquivo existe
    if (access(arquivo, F_OK) == 0) {
        //se existe, retorna se pode ser lido
        return access(arquivo, R_OK);
    }
    else
        return -1;

}

int tamanho_do_arquivo(FILE * arquivo) {
    
    // Vai até o final do arquivo
    fseek(arquivo, 0L, SEEK_END);

    // Pega a posição atual do ponteiro, ou seja, seu números de bytes
    unsigned int size = ftell(arquivo);
    
    // volta o ponteiro do arquivo para o início
    fseek(arquivo, 0L, SEEK_SET);

    return size;

}

FILE *abre_arquivo(char *nome_arquivo, char *modo) {

    char arquivo[BUFFER_IMENSO];

    strcpy(arquivo, "./");
    strcat(arquivo, nome_arquivo);

    //abre o arquivo dado com o modo dado
    FILE *arq = fopen(arquivo, modo);

    //retorna null se nao foi bem sucedido
    if (! arq) {
        perror("O arquivo não pôde ser aberto");
        return NULL;
    }

    //retorna o arquivo
    return arq;

}

int testa_existencia_diretorio(msg_t* msg_nome_dir) {

    // Testa para ver se o diretório que cliente quer criar já existe e verifica se o diretório atual tem permissão
    if ((access(msg_nome_dir->dados, F_OK) == 0) || tem_permissao() == -1){
        return 0;
    }
    return 1;

}

void manda_nack(int soquete) {

    msg_t msg_nack;
    init_mensagem(&msg_nack, 0, sequencia_global, NACK, "");
    if( ! manda_mensagem (soquete, &msg_nack)) {
        perror("Erro ao mandar nack");
    }

}

void manda_ack(int soquete) {
    msg_t msg_ack;
    init_mensagem(&msg_ack, 0, sequencia_global, ACK, "");
    if( ! manda_mensagem (soquete, &msg_ack)) {
        perror("Erro ao mandar ack");
    }
}

