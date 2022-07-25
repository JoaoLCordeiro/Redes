#ifndef __GERAL__
#define __GERAL__

#define MARC_INICIO 126

/*-----------------------------------------TIPOS-----------------------------------------------*/
#define OK      1
#define NACK    2
#define ACK     3
#define CD      6
#define LS      7
#define MKDIR   8 
#define GET     9
#define PUT     10
#define ERRO    17
#define DESC    24
#define DADOS   32
#define FIM     46
#define NA_TELA 63
/*---------------------------------------------------------------------------------------------*/

typedef struct msg_t
{
  
    int marc_inicio : 8;    //Marcador de início
    int size_msg : 6;       //Tamanho da mensagem
    int sequencia : 4;      //Número da mensagem (até 16)
    int tipo : 6;           //Tipo da mensagem
    char *dados;            //Buffer dos dados
    int paridade : 8;       //Paridade

} msg_t;

/**
 * @brief Inicializa o campo dados.
 * 
 * @param tamanho (_int_) : tamanho da mensagem do campo de dados.
 * @param dados (_char*_) : Conteúdo da mensagem.
 * 
 * @return char* : Ponteiro para a região inicializada. 
 */
char * init_dados(int tamanho, char *dados);

/**
 * @brief Inicializa os campos da mensagem.
 * 
 * @param mensagem (_msg_t_) : É a mensagem para ser inicializada.
 * @param tamanho (_int_) : Tamanho do campo de dados.
 * @param tipo_mensagem (_int_) : Tipo da mensagem.
 * @param dados (_char*_) : Conteúdo da mensagem.
 */
void init_mensagem(msg_t *mensagem, int tamanho, int tipo_mensagem, char *dados);


#endif
