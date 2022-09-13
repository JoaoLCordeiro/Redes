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
#include "servidor_lib.h"

/******************************************MAIN*************************************************/

int recebe_mensagem_server(int soquete, msg_t *mensagem) {

    while (1) {
        int retorno_func = recebe_mensagem(soquete, mensagem, DESLIGADO);

		if (retorno_func == 0) 
            perror("Erro ao receber mensagem no recebe_retorno");
		else if (retorno_func == 2){
			perror("Timeout");
			continue;
		}

        if (mensagem->marc_inicio == MARC_INICIO) {
            if (testa_paridade(mensagem))
                return mensagem->tipo;
            else
                manda_nack(soquete);
        }
        else
            manda_nack(soquete);
    }
}

/***************************************FIM MAIN************************************************/

/******************************************PUT**************************************************/
void put_dados_server(int soquete, msg_t *mensagem, char *nome_arq) {

    printf("put_dados_sevidor\n");

    char aux[128];
    //isso serve para criar outro arquivo, APENAS PARA TESTES
    strcpy(aux, "./destino_servidor/");
    strcat(aux, nome_arq);

    FILE * arq = abre_arquivo(aux, "wb+");
    if (fwrite(mensagem->dados, sizeof(char), mensagem->size_msg, arq) != mensagem->size_msg){
        perror("put_server(): Escreveu tamanho errado no servidor");
        return;
    }

    int conta_mensagens = 1;
	
    while (1){
        init_mensagem(mensagem, 0, sequencia_global, ACK, "");
        if (! manda_mensagem (soquete, mensagem))
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
				
				executa_chmod (mensagem->dados, nome_arq, "./destino_servidor/");

                init_mensagem(mensagem, 0, sequencia_global, ACK, "");
                manda_mensagem (soquete, mensagem);
                fclose(arq);
                printf("%d\n", conta_mensagens);
                return;
                break;

            default:
                break;
        }
    }
    

}


void put_tamanho_server(int soquete, char *nome_arq){
    
    //REMOVER COMENTARIO
    //aqui, o servidor deveria testar algo com o tamanho do arquivo, mas nao fizemos

    printf("put_tamanho_sevidor\n");

    //cria um ok
    msg_t mensagem;
    
    while (1){
        //cria um ok
        init_mensagem(&mensagem, 0, sequencia_global, OK, "");

        //manda um ok
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_put_servidor");

        switch (recebe_retorno(soquete, &mensagem)) {
            //se for DADOS, continua
            case DADOS:
                put_dados_server(soquete, &mensagem, nome_arq);

                //voltou da dados_server, agora dai dessa funcao
                return;
                break;
        }
    }

    return;
}


void trata_put_servidor(int soquete, msg_t* msg_put_inicial){

    printf("trata_put_sevidor\n");

    msg_t mensagem;
    
    //coloca o nome do arquivo q está em msg_put_inicial no nome_arquivo
    char nome_arquivo[BUFFER_IMENSO];

    escreve_string (soquete, nome_arquivo, msg_put_inicial);

    //verifica permissão do diretório
    if (tem_permissao() < 0){
        perror("Você não tem permissão de escrita neste diretório\n");
		manda_erro(soquete);
        return;
    }
    
    while (1){
        //cria um ok
        init_mensagem(&mensagem, 0, sequencia_global, OK, "");

        //manda um ok
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_put_servidor");

        switch (recebe_retorno(soquete, &mensagem)) {
            //se for o tamanho da mensagem, continua
            case DESC:
                put_tamanho_server(soquete, nome_arquivo);

                //voltou da função do tamanho, sai dessa funcao
                return;
                break;
        }
    }

    //sai da funcao
    return;
}

/****************************************FIM PUT***********************************************/

/*****************************************GET**************************************************/
void get_dados_server(int soquete, FILE *arq_server, int permissao){

    int contador = 1;
    printf("get_dados_server\n");

    msg_t mensagem;

    char buffer_arq[TAM_BUFFER_DADOS];
    int bytes_lidos = fread(buffer_arq, sizeof(char), TAM_BUFFER_DADOS - 1, arq_server);
    
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
            }
        }
        memset(buffer_arq, 0, TAM_BUFFER_DADOS);
        bytes_lidos = fread(buffer_arq, sizeof(char), TAM_BUFFER_DADOS - 1, arq_server);
    }

	//manda uma mensagem do tipo FIM
	char permissao_string[TAM_BUFFER_DADOS - 1];
    sprintf(permissao_string, "%d", permissao);

    init_mensagem(&mensagem, strlen(permissao_string), sequencia_global, FIM, permissao_string);

    //considerando que o cliente responde um FIM com um ACK
    while (1){
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no put_dados");

        switch (recebe_retorno(soquete, &mensagem)) {
            //se for ack, acaba
            case ACK:
                printf("get_dados_server: recebeu um ack do cliente, retornando...\n");
                printf("Contador -> %d\n", contador);
                return;
        }
    }


}


void trata_get_servidor(int soquete, msg_t* msg_get_inicial) {

    printf("trata_get_sevidor\n");

    msg_t mensagem;
    
    //coloca o nome do arquivo q está em msg_put_inicial no nome_arquivo
    char nome_arquivo[BUFFER_IMENSO];

    escreve_string (soquete, nome_arquivo, msg_get_inicial);

    //strcpy(nome_arquivo, msg_get_inicial->dados);

    //verifica se o arquivo existe e se pode lê-lo
    if (check_permissao_existencia(nome_arquivo) < 0){
        //Voltar uma mensagem de erro
        perror("O Arquivo não existe ou você não possui permissão de leitura");
		manda_erro(soquete);
        return;
    }

	int permissao = pega_permissao_arq (nome_arquivo);

    //Abre o arquivo, pega o seu tamanho, transforma o tamanho em uma string 
    FILE *arq_server = abre_arquivo(nome_arquivo, "rb");
    int tamanho = tamanho_do_arquivo(arq_server);
    char tamanho_string[TAM_BUFFER_DADOS - 1];
    sprintf(tamanho_string, "%d", tamanho);
    
    while (1){
        //cria uma mensagem com o tamanho
        init_mensagem(&mensagem, strlen(tamanho_string), sequencia_global, DESC, tamanho_string);

        //manda uma mensagem com o tamanho
        if (! manda_mensagem (soquete, &mensagem))
            perror("Erro ao enviar mensagem no trata_get_servidor");

        // Recebe um ok ou erro do cliente [get_tamanho_cliente]
        switch (recebe_retorno(soquete, &mensagem)) {

            //se for o tamanho da mensagem, continua
            case OK:
                get_dados_server(soquete, arq_server, permissao);
                fclose(arq_server);
                //voltou da função dos dados, sai dessa funcao
                return;
            break;

            case ERRO:
                return;
            break;
        }
    }

    //sai da funcao
    return;

}
/****************************************FIM GET***********************************************/

/*****************************************MKDIR************************************************/
void trata_mkdir_servidor(int soquete, msg_t* msg_nome_diretorio){

    printf("trata_mkdir_sevidor\n");
    
    char nome_dir[BUFFER_IMENSO];
    escreve_string (soquete, nome_dir, msg_nome_diretorio);

    //cria um ok
    msg_t mensagem;
    
    if (! testa_existencia_diretorio(nome_dir)) {
        init_mensagem(&mensagem, 0, sequencia_global, ERRO, "");
    }
    else {
        init_mensagem(&mensagem, 0, sequencia_global, OK, "");
        
        FILE * saida_comando;
        char comando[BUFFER_IMENSO] = "mkdir ";
        
        strcat(comando, nome_dir);
        saida_comando = popen(comando, "r");

        pclose (saida_comando);
    }

    //manda um ok ou um erro
    if (! manda_mensagem (soquete, &mensagem))
        perror("Erro ao enviar mensagem no trata_put_servidor");
    switch (recebe_retorno(soquete, &mensagem)) {
        
        //se for ACK, acabou
        case ACK:         
            return;
            break;
    }

    return;

}
/******************************************FIM*************************************************/

/******************************************LS**************************************************/
void trata_ls_servidor(int soquete, msg_t *mensagem){

    printf("trata_ls_servidor\n");


    msg_t msg;

    // Testa permissão do diretório corrente
    // Se não tem permissão
    if ( access("./", R_OK) != 0 ) {
        init_mensagem(&msg, 0, sequencia_global, ERRO, "");

        //Manda mensagem com o erro
        if (! manda_mensagem(soquete, &msg)) {
            perror("Não foi possível enviar ERRO no trata_ls_servidor");
        }

        //Espera resposta do cliente (ACK)
        while(1) {
            switch (recebe_retorno(soquete, &msg)){
                
                // Se recebeu ack acaba
                case ACK:
                    return;
                    break;
            }
        }
    }
    //Se tem permissão

    //Executam comando e salva a saída no arquivo saida_comando
    FILE * saida_comando = popen(mensagem->dados, "r");
    char buffer_arq[TAM_BUFFER_DADOS];

    int bytes_lidos = fread(buffer_arq, sizeof(char), TAM_BUFFER_DADOS - 1, saida_comando);
    
    while (bytes_lidos != 0){

        init_mensagem(&msg, bytes_lidos, sequencia_global, NA_TELA, buffer_arq);
        int ack = 0;
        while (! ack){
            if (! manda_mensagem (soquete, &msg))
                perror("Erro ao enviar mensagem no put_dados");

            switch (recebe_retorno(soquete, &msg)) {
                
                //se for ack, quebra o laço interno e vai pro laço externo pegar mais dados
                case ACK:
                    ack = 1;
                    break;
            }
        }
        memset(buffer_arq, 0, TAM_BUFFER_DADOS);
        bytes_lidos = fread(buffer_arq, sizeof(char), TAM_BUFFER_DADOS - 1, saida_comando);
    }
    pclose(saida_comando);

    //manda uma mensagem do tipo FIM
    init_mensagem(&msg, 0, sequencia_global, FIM, "");

    //considerando que o cliente responde um FIM com um ACK
    while (1){
        if (! manda_mensagem (soquete, &msg))
            perror("Erro ao enviar mensagem no put_dados");

        switch (recebe_retorno(soquete, &msg)) {
            //se for ack, acaba
            case ACK:
                printf("trata_ls_server: recebeu um ack do cliente, retornando...\n");
                return;
        }
    }


}
/***************************************FIM LS*************************************************/

/******************************************CD**************************************************/
void trata_cd_servidor(int soquete, msg_t *mensagem){

    char nome_dir[BUFFER_IMENSO];

    escreve_string (soquete, nome_dir, mensagem);

    msg_t msg;
    if ( chdir (nome_dir) == -1) {

        init_mensagem(&msg, 0, sequencia_global, ERRO, "");

    }
    else {
        init_mensagem(&msg, 0, sequencia_global, OK, "");
    }
                //Manda mensagem com o erro
    if (! manda_mensagem(soquete, &msg)) {
        perror("Não foi possível enviar o retorno no trata_cd_servidor");
    }
    //Espera resposta do cliente (ACK)
    while(1) {
        switch (recebe_retorno(soquete, &msg)){
            
            // Se recebeu ack acaba
            case ACK:
                return;
                break;
        }
    }


        
}
/***************************************FIM CD*************************************************/
