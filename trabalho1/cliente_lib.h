#ifndef __CLIENT_LIB__
#define __CLIENT_LIB__

#define MAX_SIZE_MSG 64

#include "geral.h"

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

/**
 * @brief Manda um arquivo para o servidor
 * 
 * @param soquete (int) : Número do soquete;
 * @param file_name (_char*_) : Nome do arquivo;
 * @return int : 1 se deu certo. < 0 se deu algum erro.
 */
int put_client (int soquete, char *file_name);





#endif