#ifndef __CLIENT_LIB__
#define __CLIENT_LIB__

#define MAX_SIZE_MSG 64

#include "geral.h"

/**
 * @brief Retorna o tamanho do arquivo.
 * 
 * @param arquivo (_FILE*_) : Ponteiro do descritor do arquivo
 * @return int : tamanho do arquivo
 */
int file_size (FILE *arquivo);

/***/
void trata_put (int soquete);

/**
 * @brief Envia o nome do arquivo para o put
 * 
 * @param file_name (_char*_) : Nome do arquivo.
 * @return int : 1 se deu certo. < 0 se deu algum erro
 */
int envia_nome_put (char *file_name);

/**
 * @brief Manda um arquivo para o servidor
 * 
 * @param soquete (int) : Número do soquete;
 * @param file_name (_char*_) : Nome do arquivo;
 * @return int : 1 se deu certo. < 0 se deu algum erro.
 */
int put_client (int soquete, char *file_name);


int recebe_retorno_put(int soquete, msg_t *mensagem);


#endif