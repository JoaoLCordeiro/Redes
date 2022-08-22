#ifndef __GERAL__
#define __GERAL__

#define TAM_BUFFER_DADOS 64
#define BUFFER_IMENSO 65536

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


extern int sequencia_global;


typedef struct msg_t
{
  
    int marc_inicio : 8;    //Marcador de início
    int size_msg : 6;       //Tamanho da mensagem
    int sequencia : 4;      //Número da mensagem (até 16)
    int tipo : 6;           //Tipo da mensagem
    char dados[64];         //Buffer dos dados
    int paridade : 8;       //Paridade

} msg_t;

/***/
int manda_mensagem (int soquete, msg_t *mensagem);

/***/
int recebe_mensagem (int soquete, msg_t *mensagem);

/**
 * @brief Calcula a paridade dos dados
 * 
 * @param tamanho (_int_) : Quantidade de bytes dos dados
 * @param dados (_char*_) : Dados
 * @return char : paridade dos dados
 */
char calcula_paridade (int tamanho, char * dados);

/**
 * @brief Testa para ver se a paridade confere
 * 
 * @param mensagem (_msg_t*_) : Mensagem a ser testada
 * @return int : 1 se confere, 0 senão.
 */
int testa_paridade(msg_t *mensagem);

/**
 * @brief Compara o comando que o usuário digitou com os
 * comandos existentes.
 * 
 * @param comando (_char*_) : Comando que o usuário digitou 
 * @return int : Nº do comando se achou o comando, 0 caso não tenha achado
 */
int compara_comando(char *comando);


/**
 * @brief Inicializa o campo dados.
 * 
 * @param tamanho (_int_) : tamanho da mensagem do campo de dados.
 * @param buffer_dados (_char*_) : buffer que guarda a mensagem.
 * @param dados (_char*_) : Conteúdo da mensagem.
 * 
 * @return char* : Ponteiro para a região inicializada. 
 */
void init_dados(int tamanho, char *buffer_dados, char *dados);


/**
 * @brief Imprime mensagem
 * 
 * @param mensagem (_msg_t*_) : mensagem
 */
void imprime_mensagem(msg_t *mensagem);

 
/**
 * @brief Inicializa os campos da mensagem.
 * 
 * @param mensagem (_msg_t_) : É a mensagem para ser inicializada.
 * @param tamanho (_int_) : Tamanho do campo de dados.
 * @param sequencia (_int_) : Sequência da mensagem.
 * @param tipo_mensagem (_int_) : Tipo da mensagem.
 * @param dados (_char*_) : Conteúdo da mensagem.
 */
void init_mensagem(msg_t *mensagem, int tamanho, int sequencia, int tipo_mensagem, char *dados);

/**
 * @brief Envia a mensagem dando ok ao cliente
 * 
 * @param soquete (_int_) : Soquete
 * 
 */
void envia_ok(int soquete);


int recebe_retorno(int soquete, msg_t *mensagem);

/**
 * @brief Verifica se o diretório tem permissão.
 * 
 * @return int : 0 se tem permissão; < 0 se não tem permissão.
 */
int tem_permissao();

/**
 * @brief Testa para ver se o arquivo existe.
 * 
 * @return int : 0 se o arquivo existe, < 0 senão existe.
 */
int check_permissao_existencia(char *);


/**
 * @brief Abre o arquivo e retorna o seu tamanho.
 * 
 * @param arquivo (_FILE*_) : Descritor do arquivo.
 * @return int : tamanho do arquivo.
 */
int tamanho_do_arquivo(FILE *);


/**
 * @brief Abre o arquivo e retorna seu descritor.
 * 
 * @param nome_arquivo (_char*_) : Nome do arquivo.
 * @return FILE* : descritor do arquivo aberto.
 */
FILE *abre_arquivo(char *nome_arquivo);



#endif
