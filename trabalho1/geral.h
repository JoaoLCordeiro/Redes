#ifndef __GERAL__
#define __GERAL__

#define TIMEOUT 1000*3

#define TAM_BUFFER_DADOS 64
#define BUFFER_IMENSO 65536

#define MARC_INICIO 126

#define DESLIGADO 0
#define LIGADO 1

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


#define CDL      61     // cd local
#define LSL      71     // ls local
#define MKDIRL   81     // mkdir local
/*---------------------------------------------------------------------------------------------*/


extern int sequencia_global;


typedef struct msg_t
{
  
    unsigned int marc_inicio : 8;    //Marcador de início
    unsigned int size_msg : 6;       //Tamanho da mensagem
    unsigned int sequencia : 4;      //Número da mensagem (até 16)
    unsigned int tipo : 6;           //Tipo da mensagem
    char dados[64];         //Buffer dos dados
    unsigned int paridade : 8;       //Paridade

} msg_t;

/**
 * @brief Manda mensagem.
 * 
 * @param soquete (int) : Soquete
 * @param mensagem (msg_t *) : Mensagem a ser enviada
 * @return int 0 se erro, 1 caso contrário
 */
int manda_mensagem (int soquete, msg_t *mensagem, int flag_mask);

/**
 * @brief Recebe mensagem.
 * 
 * @param soquete (int) : Soquete
 * @param mensagem (msg_t *) : Mensagem recebida
 * @return int : 0 se erro, 1 caso contrário 
 */
int recebe_mensagem (int soquete, msg_t *mensagem, int timeout, int flag_mask);

/**
 * @brief Calcula a paridade dos dados
 * 
 * @param tamanho (_int_) : Quantidade de bytes dos dados
 * @param dados (_char*_) : Dados
 * @return char : paridade dos dados
 */
unsigned char calcula_paridade (int tamanho, char * dados);

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
 * @brief Recebe a mensagem e retorna o seu tipo. Em caso de erro,
 * retorna NACK. Se receber um NACK, manda novamente a última mensagem.
 * 
 * @param soquete (int) : Soquete
 * @param mensagem (msg_t*) : Última mensagem.
 * @return int 
 */
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
 * @param modo (_char*_) : Modo como você quer abrir o arquivo.
 * 
 * @return FILE* : descritor do arquivo aberto.
 */
FILE *abre_arquivo(char *nome_arquivo, char *modo);


/**
 * @brief Testa se o diretório que o cliente quer criar já existe,
 * e se ele tem permissão de criá-lo no dir correte.
 * 
 * @param nome_dir (char*) : Nome do diretorio
 * @return int : 0 se der algum erro, 1 caso contrário
 */
int testa_existencia_diretorio(char* nome_dir);


/**
 * @brief Manda um nack 
 *  
 * @param soquete (int) : Soquete
 */
void manda_nack(int soquete);

/**
 * @brief Manda um ack
 * 
 * @param soquete (int) : soquete
 */
void manda_ack(int soquete);

/**
 * @brief Manda um erro e espera um ack do cliente
 * 
 * @param soquete (int) : soquete
 */
void manda_erro(int soquete);

/**
 * @brief Pega todas as mensagens com nome e junta em uma string
 * 
 * @param soquete (int) : soquete
 * @param string_destino (char*) : destino onde sera feita a string
 * @param mensagem (mst_t*) : mensagem recebida anteriormente
 */
void escreve_string (int soquete, char* string_destino, msg_t *mensagem);

/**
 * @brief Pega uma string e manda em varias mensagens
 * 
 * @param soquete (int) : soquete
 * @param nome (char*) : string a ser enviada
 * @param tipo (int) : tipo da mensagem
 */
void manda_nome (int soquete, char* nome, int tipo);


int pega_permissao_arq (char *nome_arq);

void executa_chmod (char *permissao, char *nome_arq, char *diretorio);

#endif
