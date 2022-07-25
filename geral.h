#ifndef __GERAL__
#define __GERAL__

typedef struct msg_t
{
  
    int marc_inicio : 8;    //Marcador de início
    int size_msg : 6;       //Tamanho da mensagem
    int sequencia : 4;      //Número da mensagem (até 16)
    int tipo : 6;           //Tipo da mensagem
    char *dados;            //Buffer dos dados
    int paridade : 8;       //Paridade

} msg_t;



#endif
