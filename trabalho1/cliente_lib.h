#ifndef __CLIENT_LIB__
#define __CLIENT_LIB__

#define MAX_SIZE_MSG 64

#include "geral.h"

/******************************************GET*************************************************/

/**
 * @brief Envia o arquivo para o servidor
 * 
 * @param soquete (_int_) : soquete
 * @param arq (_FILE*_) : Descritor do arquivo
 */
void put_dados_cliente (int soquete, FILE * arq);

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

/**************************************FIM GET*************************************************/

/**
 * @brief Trata do get do cliente para o servidor
 *  
 * @param soquete (_int_) : Soquete
 */
void trata_get_cliente(int soquete);




#endif