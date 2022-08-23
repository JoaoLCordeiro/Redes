#ifndef __SERVER_LIB__
#define __SERVER_LIB__

#include "geral.h"


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

/**
 * @brief 
 * 
 * @param soquete 
 * @param mensagem 
 * @return int | 2 se der ERRO,  
 */
int recebe_mensagem_server(int soquete, msg_t *mensagem);
#endif
