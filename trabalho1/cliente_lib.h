#ifndef __CLIENT_LIB__
#define __CLIENT_LIB__

#define MAX_SIZE_MSG 64

#include "geral.h"

/******************************************PUT*************************************************/
/**
 * @brief Envia o arquivo para o servidor
 * 
 * @param soquete (_int_) : soquete
 * @param arq (_FILE*_) : Descritor do arquivo
 * @param permissao (_int_) : Permissao do arquivo
 */
void put_dados_cliente (int soquete, FILE * arq, int permissao);

/**
 * @brief Trata do put do cliente para o servidor
 * 
 * @param soquete (_int_) : Nº do soquete
 */
void trata_put_cliente (int soquete);

/**
 * @brief 
 * 
 * @param soquete 
 * @param nome_arquivo 
 */
void put_tamanho_cliente (int soquete, char *nome_arquivo);
/**************************************FIM PUT************************************************/

/*****************************************GET*************************************************/

/**
 * @brief Trata dos dados vindo do servidor.
 * 
 * @param soquete (int) : soquete.
 * @param mensagem (msg_t *) : Mensagem contendo os dados.
 * @param nome_arquivo (char *) : O nome do arquivo para ser criado.
 */
void get_dados_cliente(int soquete, msg_t* mensagem, char *nome_arquivo);

/**
 * @brief Trata do tamanho vindo do servidor
 * 
 * @param soquete (_int_) : soquete
 * @param nome_arquivo (_char *_) : Nome do arquivo para ser criado
 * @param msg_tam_server (msg_t *) : Mensagem contendo o nome do arquivo
 */
void get_tamanho_cliente(int soquete, char *nome_arquivo, msg_t *msg_tam_server);

/**
 * @brief Trata do get do cliente para o servidor
 *  
 * @param soquete (_int_) : Soquete
 */
void trata_get_cliente(int soquete);
/**************************************FIM GET************************************************/

/***************************************MKDIR*************************************************/

/**
 * @brief Trata do comando mkdir por parte do cliente
 * 
 * @param soquete (int) : Soquete
 */
void trata_mkdir_cliente(int soquete);
/*************************************FIM MKDIR***********************************************/

/******************************************LS*************************************************/

/**
 * @brief Trata do LS do cliente para o servido.
 * 
 * @param soquete (_int_) : soquete
 * @param buffer_c (_char *_) : buffer contendo o comando.
 */
void trata_ls_cliente(int soquete, char * buffer_c);
/***************************************FIM LS************************************************/

/******************************************CD*************************************************/

/**
 * @brief Trata do CD do cliente para o servidor.
 * 
 * @param soquete (_int_) : soquete
 * @param buffer_c (_char *_) : buffer contendo o comando.
 */
void trata_cd_cliente(int soquete);
/***************************************FIM CD************************************************/


/***********************************FUNÇÕES LOCAIS*********************************************/

/**
 * @brief Trata do comando ls local.
 * 
 * @param comando (_char *_) : Comando
 */
void trata_local_ls_cliente(char *comando);

/**
 * @brief Trata do comando cd local.
 * 
 */
void trata_local_cd_cliente();

/**
 * @brief Trata do comando mkdir local.
 * 
 */
void trata_local_mkdir_cliente();
/**********************************FIM FUNC LOCAIS*********************************************/

#endif