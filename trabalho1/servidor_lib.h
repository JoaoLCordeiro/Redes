#ifndef __SERVER_LIB__
#define __SERVER_LIB__

#include "geral.h"


/**
 * @brief 
 * 
 * @param soquete 
 * @param mensagem 
 * @return int | 2 se der ERRO,  
 */
int recebe_mensagem_server(int soquete, msg_t *mensagem);

/***********************************************PUT*******************************************/

/**
 * @brief 
 * 
 * @param soquete (_int_) : 
 * @param mensagem (_msg_t*_) :
 * @param nome_arq (_char*_) : Nome arquivo
 */
void put_dados_server(int soquete, msg_t *mensagem, char *nome_arq);

/**
 * @brief 
 * 
 * @param soquete (_int_)
 * @param nome_arq (_char*_)
 */
void put_tamanho_server(int soquete, char *nome_arq);

/**
 * @brief 
 * 
 * @param soquete 
 * @param mensagem 
 */
void trata_put_servidor(int soquete, msg_t* mensagem);
/****************************************FIM PUT***********************************************/


/*****************************************GET**************************************************/

/**
 * @brief Abre o arquivo e manda ele para o cliente.
 * 
 * @param soquete (int) : Soquete
 * @param arq_server (FILE *) : Arquivo que o cliente pediu
 */
void get_dados_server(int soquete, FILE *arq_server, int permissao);

/**
 * @brief Se o servidor recebeu um get,
 * trata esse get.
 * 
 * @param soquete  (_int_) : soquete
 * @param msg_get_inicial (msg_t*) : Mensagem com o nome do arquivo requerido
 */
void trata_get_servidor(int soquete, msg_t* msg_get_inicial);
/****************************************FIM GET***********************************************/

/*****************************************MKDIR************************************************/

/**
 * @brief Se o servidor recebeu um mkdir,
 * trata esse mkdir.
 * 
 * @param soquete (int) : Soquete
 * @param msg_mkdir_inicial (msg_t*) : Mensagem com o nome do arquivo para criar
 */
void trata_mkdir_servidor(int soquete, msg_t* msg_mkdir_inicial);
/**************************************FIM  MKDIR**********************************************/

/******************************************LS*************************************************/

/**
 * @brief Se o servidor recebeu um ls, trata esse LS
 * 
 * @param soquete (int) : Soquete
 * @param mensagem (msg_t*) : Mensagem contendo a mensagem com o comando
 */
void trata_ls_servidor(int soquete, msg_t *mensagem);

/***************************************FIM LS************************************************/

/******************************************CD*************************************************/

/**
 * @brief Se o servidor recebeu um cd, trata esse CD
 * 
 * @param soquete (int) : Soquete
 * @param mensagem (msg_t*) : Mensagem contendo a mensagem com o comando
 */
void trata_cd_servidor(int soquete, msg_t *mensagem);

/***************************************FIM CD************************************************/

#endif
