#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <poll.h>

#include <sys/types.h>
//#include <linux/types.h>

#include <sys/socket.h>
//#include <linux/socket.h>

#include "geral.h"

int manda_mensagem (int soquete, msg_t *mensagem){
    if (send(soquete, mensagem, sizeof(msg_t), 0) < 0){
        return 0;
	}
    
	//printf("mandou tipo %d e sequencia %d\n\n", mensagem->tipo, mensagem->sequencia);

    if (sequencia_global >= 15)
            sequencia_global = 1;
        else
            sequencia_global++;
    return 1;
}

int recebe_mensagem (int soquete, msg_t *mensagem){
    while (1){
		//cuida do timeout
		struct pollfd fds;

		fds.fd = soquete;
		fds.events = POLLIN;

		int retorno_poll = poll(&fds, 1, TIMEOUT);

		if (retorno_poll == 0)
			return 2;
		else if (retorno_poll < 0)
			return 0;
		
        if (recv(soquete, mensagem, sizeof(msg_t), 0) < 0)
            return 0;
    
        if (mensagem->sequencia != sequencia_global) {
            continue;
        }

		//printf("recebeu tipo %d e sequencia %d\n\n", mensagem->tipo, mensagem->sequencia);

        if (sequencia_global >= 15)
            sequencia_global = 1;
        else
            sequencia_global++;
        return 1;
    }
}

unsigned char calcula_paridade (int tamanho, char * dados){
    
    // Faz um XOR dos bits para cada byte da mensagem.
    unsigned char aux = dados[0];
    for (int i = 1; i < tamanho; i++){
        aux = aux ^ dados[i];
	}

    return aux;
}

int testa_paridade(msg_t*mensagem) {

    // Calcula a paridade da mensagem que chegou
    unsigned int paridade = (unsigned int)calcula_paridade(mensagem->size_msg, mensagem->dados);

    // Testa a paridade calculada com a paridade da mensagem que chegou
    // Se ela é igual ou o seu complemente de dois ela chegou certo (teoricamente)
    if (mensagem->paridade - paridade == 256 || paridade == mensagem->paridade)
        return 1;
    printf("PARIDADE CALCULADA %d ----- PARIDADE DA MENSAGEM %d\n", paridade, mensagem->paridade);

	printf("Chegou:\n");
	imprime_mensagem(mensagem);
	printf("\n");
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
	printf("Tam Mem -> %ld\n", strlen(mensagem->dados));
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
	msg_t mensagem_aux;

	mensagem_aux.size_msg	= mensagem->size_msg;
	mensagem_aux.tipo		= mensagem->tipo;
	memcpy(mensagem_aux.dados, mensagem->dados, mensagem->size_msg);
	
    while (1) {
        // Recebe uma mensagem
		int retorno_func = recebe_mensagem (soquete, mensagem);

        if (retorno_func == 0) 
            perror("Erro ao receber mensagem no recebe_retorno");
		else if (retorno_func == 2){
			perror("Timeout");
			continue;
		}
        
        // Verifica se o marcador de início e a paridade são os corretos
        if ((mensagem->marc_inicio == MARC_INICIO)/* && (mensagem->sequencia != 4)*/) {
            //Testa a paridade
            if (testa_paridade(mensagem)) {

                //se for um NACK, reenvia a mensagem
                if (mensagem->tipo == NACK){
                    //aqui nao damos return pro laço recomeçar e esperar mais uma resposta
                    char buffer_aux[TAM_BUFFER_DADOS];

					memset(buffer_aux, 0, TAM_BUFFER_DADOS);
                    memcpy(buffer_aux, mensagem_aux.dados, mensagem_aux.size_msg);

					init_mensagem(&mensagem_aux, mensagem_aux.size_msg, sequencia_global, mensagem_aux.tipo, buffer_aux);

					//printf("Remandando o seguinte:\n");
					//imprime_mensagem(&mensagem_aux);
					//printf("\n");

                    if (! manda_mensagem (soquete, &mensagem_aux))
                        perror("Erro ao re-mandar mensagem no recebe_retorno_put");
                }

                // Senão retorna o tipo    
                else {
                    return mensagem->tipo;
                }
            
            }
            else
            //retorna NACK para mensagens com erro no marcador ou na paridade
                manda_nack(soquete);    
        }
        else
            manda_nack(soquete);
		
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

int testa_existencia_diretorio(char* nome_dir) {

    // Testa para ver se o diretório que cliente quer criar já existe e verifica se o diretório atual tem permissão
    if ((access(nome_dir, F_OK) == 0) || tem_permissao() == -1){
        return 0;
    }
    return 1;

}

void manda_nack(int soquete) {
	printf("Mandou um nack\n");

    msg_t msg_nack;
    init_mensagem(&msg_nack, 0, sequencia_global, NACK, "");
    if( ! manda_mensagem (soquete, &msg_nack)) {
        perror("Erro ao mandar nack");
    }

}

void manda_ack(int soquete) {
	printf("Mandou um ack\n");

    msg_t msg_ack;
    init_mensagem(&msg_ack, 0, sequencia_global, ACK, "");
    if( ! manda_mensagem (soquete, &msg_ack)) {
        perror("Erro ao mandar ack");
    }
}

void manda_erro(int soquete) {
	printf("Mandou um erro\n");

	while(1){
    	msg_t msg_erro;
    	init_mensagem(&msg_erro, 0, sequencia_global, ERRO, "");
    	if( ! manda_mensagem (soquete, &msg_erro)) {
    	   perror("Erro ao mandar ack");
    	}

		switch (recebe_retorno(soquete, &msg_erro)){
			case ACK:
				return;
				break;

			case NACK:
				break;
		}
	}
}

void escreve_string (int soquete, char* string_destino, msg_t *mensagem){
    printf("escreve_string\n");

    //pega a primeira mensagem com nome
    strcpy (string_destino, mensagem->dados);
    msg_t mensagem_ok;
    
    while(1){
        //manda um ok
        init_mensagem(&mensagem_ok, 0, sequencia_global, OK, "");
        if (! manda_mensagem(soquete, &mensagem_ok))
            perror ("escreve_string: erro no manda mensagem\n");

        //recebe mais uma mensagem
        switch (recebe_retorno(soquete, mensagem)){

            //se tiver mais nome, concatena
            case DESC:
                strcat(string_destino, mensagem->dados);
                printf("escreve_string: concatenou\n");
                break;

            //se nao, retorna
            case FIM:
                return;

            default:
                printf("erro desconhecido no escreve_string\n");
                break;
        }
    }
}

void manda_nome (int soquete, char* nome, int tipo){
    printf("manda_nome\n");

    msg_t mensagem;

    if (strlen(nome) <= 63){
        printf("manda_nome: entrou no string <= 63\n");

        //simplesmente manda o nome em uma unica mensagem
        init_mensagem(&mensagem, strlen(nome), sequencia_global, tipo, nome);
        manda_mensagem (soquete, &mensagem);

        if (recebe_retorno(soquete, &mensagem) == OK)
            return;
        
        perror ("manda_nome: erro inesperado em mensagem pequena\n");
        return;
    }
    else{
        printf("manda_nome: entrou no string > 63\n");

        char buffer[TAM_BUFFER_DADOS];
        memcpy (buffer, nome, 63);

        //manda a primeira mensagem
        init_mensagem (&mensagem, 63, sequencia_global, tipo, buffer);
        manda_mensagem (soquete, &mensagem);

        int cont = 1;
        printf("manda_nome: enviou mensagem %d\n", cont);

        if (recebe_retorno(soquete, &mensagem) != OK){
            perror ("manda_nome: erro inesperado em mensagem grande\n");
            return;
        }

        //avança o ponteiro
        nome = &(nome[63]);
        memset (buffer, 0, 64);

        //manda o resto das mensagens menos a ultima
        while (63 - strlen(nome) < 0){
            memcpy (buffer, nome, 63);

            init_mensagem (&mensagem, 63, sequencia_global, DESC, buffer);
            manda_mensagem (soquete, &mensagem);

            cont++;
            printf("manda_nome: enviou mensagem %d\n", cont);

            if (recebe_retorno(soquete, &mensagem) != OK){
                perror ("manda_nome: erro inesperado em mensagem grande\n");
                return;
            }

            nome = &(nome[63]);
            memset (buffer, 0, 64);
        }

        //manda a ultima mensagem
        init_mensagem (&mensagem, strlen(nome), sequencia_global, DESC, nome);
        manda_mensagem (soquete, &mensagem);

        cont++;
        printf("manda_nome: enviou mensagem %d\n", cont);

        if (recebe_retorno(soquete, &mensagem) == OK)
            return;
        
        perror ("manda_nome: erro inesperado em mensagem grande\n");
        return;
    }
}