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

/**
 * @brief 
 * 
 * @param soquete 
 * @param mensagem 
 */
void trata_put_servidor(int soquete, msg_t* mensagem);


#endif
