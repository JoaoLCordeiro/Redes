import random
import socket
import pickle
import copy
from time import sleep
from Mensagem import Mensagem
#from 
import lib_mensagem as lib_m

BUFFERSIZE = 4096
UDP_IP = "127.0.0.1"
BASTAO = False
UDP_PORT_IN = None
UDP_PORT_OUT = None
FICHAS = 10

valor_combinacao = {
    1: (2, 'dupla')
    ,2: (3, 'trio')
    ,3: (4, 'd_dupla')
    ,4: (5, 'full_house')
    ,5: (7, 'seq_alta')
    ,6: (7, 'seq_baixa')
    ,7: (10, 'quadra')
    ,8: (15, 'quinteto')
}

portas = {
    1 : 1307
    ,2 : 8600
    ,3 : 1456
    ,4 : 2010
}

def resultado(combinacao, dados):
    """De acordo com a combinação e a jogada nos dados
    retorna sua pontuação.

    Args:
        combinacao (_str_): Combinação que o jogador escolheu
        dados (_list_): Dados que o jogador jogou.

    Returns:
        _int_: Valor da jogada
    """

    aposta = valor_combinacao[int(combinacao)]
    if aposta[1] == 'dupla':
        for dado in dados:
            if dados.count(dado) == 2:
                return aposta[0]
        return 0

    if aposta[1] == 'trio':
        for dado in dados:
            if dados.count(dado) == 3:
                return aposta[0]
        return 0

    if aposta[1] == 'd_dupla':

        contador = 0
        l_valores_dados = list([1,2,3,4,5,6])

        for valor_dado in l_valores_dados:
            if dados.count(valor_dado) == 2:
                  contador += 1
                  if contador == 2:
                    return aposta[0]
        return 0

    if aposta[1] == 'full_house':

        contador = 0
        l_valores_dados = list([1,2,3,4,5,6])

        for valor_dado in l_valores_dados:
            if dados.count(valor_dado) == 2:
                contador += contador + 2
            elif dados.count(valor_dado) == 3:
                contador += contador + 3

            if contador == 5:
                return aposta[0]
        return 0

    if aposta[1] == 'seq_alta':

        contador = 0
        l_valores_dados = list([2,3,4,5,6])

        for valor_dado in l_valores_dados:
            if dados.count(valor_dado) == 1:
                contador += contador + 1
        if contador == 5:
            return aposta[0]
        return 0

    if aposta[1] == 'seq_baixa':

        contador = 0
        l_valores_dados = list([1,2,3,4,5])

        for valor_dado in l_valores_dados:
            if dados.count(valor_dado) == 1:
                contador += contador + 1
        if contador == 5:
            return aposta[0]
        return 0

    if aposta[1] == 'quadra':
        for dado in dados:
            if dados.count(dado) == 4:
                return aposta[0]
        return 0

    if aposta[1] == 'quinteto':
        for dado in dados:
            if dados.count(dado) == 5:
                return aposta[0]
        return 0

def joga_dados(msg):
    """Função que toma conta da jogada do jogador

    Args:
        msg (_Mensagem_): mensagem contendo a maior aposta e seu jogador.

    Returns:
        _Mensagem_: mensagem contendo a jogada.
    """

    print("Você vai jogar!!!", end="\n")
    sleep(1)
    print("Rodando os dados ...", end='\n')
    print("Resultado:", end=" ")

    # Joga os dados (i.e. escolhe aleatoriamente entre 1 a 6 cinco vezes)
    for i in range(0, 5):
        numero = random.randint(1, 6)
        print(f"[{numero}]", end=" ")
        msg.dados.append(numero)

    # pula linha
    print("")

    # faz uma vetor que guarda se aquele dado já foi travado
    travados = list([False, False, False, False, False])

    resposta = input("Você quer travar algum dos dados? 'Y' para sim, 'N' para não\n> ")
    contador = 1
    
    # Se o jogador digitou sim e o contador é menor que 2
    while resposta.upper() == "Y" and contador <= 2:

        linha = input("Quais dados você deseja travar?\n> ").split()
        # print(linha)

        # Trava os dados que ele escolheu
        for dado in linha:
            travados[int(dado)-1] = True

        # Rejoga o restante
        for i in range(0, 5):
            if (not travados[i]):
                msg.dados[i] = random.randint(1,6)

        print("Resultado: ", end="\n")
        for i in range(0, 5):
            print(f"[{msg.dados[i]}]", end=" ")
        print()

        if contador != 2:
            resposta = input("Você quer travar algum dos dados? 'Y' para sim, 'N' para não\n> ")
        contador += 1

    global FICHAS
    
    # Atualiza o saldo do jogador
    FICHAS = FICHAS - msg.saldo + resultado(msg.combinacao, msg.dados)
    
    # Atualiza a mensagem
    msg.saldo = FICHAS

    msg.paridade = lib_m.calcula_paridade(msg)
    return msg

def testa_paridade(msg):

    aux = copy.copy(msg)
    aux.paridade = 0

    # pega a paridade da mensagem que chegou
    paridade_aux = lib_m.calcula_paridade(aux)

    # verifica se as paridades conferem

    if paridade_aux != msg.paridade:
        return False

    return True

def detecta_erro(msg):
    
    if msg.marcador_inicio != 126:
        return True
    if not testa_paridade(msg):
        return True

    return False

def recebe_mensagem():
    mensagem_bytes, addr = sock.recvfrom(BUFFERSIZE)
    mensagem = pickle.loads(mensagem_bytes)
    
    if mensagem.jogador == -1:
        envia_mensagem(mensagem)
        termina_jogo(0)

    if detecta_erro(mensagem):
        mensagem = lib_m.mensagem_erro()
        envia_mensagem(mensagem)
        termina_jogo(0)
        
    return mensagem

def envia_mensagem(msg):
    mensagem_bytes = pickle.dumps(msg)
    sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))

def termina_jogo(flag):
    if flag == 0:
        print("Erro detectado nas mensagens da rede")
        print("Terminando...")
    else:
        print(f"O jogador {flag} perdeu o jogo")
        print("O jogo será finalizado...")

    exit(1)

def imprime_saldo(mensagem):
    print("")
    print(f"O jogador {mensagem.jogador} apostou na combinação {mensagem.combinacao} e sua jogada foi {mensagem.dados}", end="\n")
    print(f"Ele está com o saldo {mensagem.saldo}.", end="\n")
    print("\nEsta rodada acabou, próxima rodada...\n\n")

def devolve_porta():
    """De acordo com o jogador retorna a porta utilizada

    Raises:
        KeyError: Caso o jogador escolha um índici fora do range

    Returns:
        _int_: A máquina (i.e jogador)
        _int_: Porta de entrada
        _int_: Porta de saída
    """
    maquina = input("Qual sua máquina?\n> ")
    if int(maquina) in range(1, 4+1):
        return int(maquina), portas[int(maquina)], portas[(int(maquina) % 4) + 1]

    raise KeyError("Indice fora do range")


if __name__ == "__main__":

    mensagem = Mensagem()

    jogador, UDP_PORT_IN, UDP_PORT_OUT = devolve_porta()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((UDP_IP, UDP_PORT_IN))

    if jogador == 1:
        BASTAO = True

    while (True):

        if BASTAO:
            mensagem = lib_m.mensagem_combinacao(1,jogador)

            # Manda a mensagem de combinação inicial da rodada
            # e recebe a mensagem com a maior aposta e seu jogador
            envia_mensagem(mensagem)
            mensagem = recebe_mensagem()

            if mensagem.jogador == jogador:

                # O mesmo joga
                mensagem = joga_dados(mensagem)

                # envia a mensagem informando os pontos do jogador
                envia_mensagem(mensagem)
            else:
                # Repassou a mensagem
                envia_mensagem(mensagem)

            # recebendo a pontuação do jogador
            mensagem = recebe_mensagem()

            imprime_saldo(mensagem)
            
            # Repassando a pontuação do jogador
            envia_mensagem(mensagem)
            
            if mensagem.saldo <= 0:
                termina_jogo(mensagem.jogador)
            
            # mensagem_bytes, addr = sock.recvfrom(BUFFERSIZE)
            mensagem = recebe_mensagem()
            
            mensagem = lib_m.mensagem_bastao(jogador)
            BASTAO = False
            
            
            # Manda a mensagem do bastão
            envia_mensagem(mensagem)
        else:

            mensagem = recebe_mensagem()

            if mensagem.bastao:
                mensagem = recebe_mensagem()

            # Enviando a mensagem da aposta atual mais o jogador
            mensagem = lib_m.mensagem_aumenta_aposta(mensagem, jogador)
            envia_mensagem(mensagem)
            
            # Recebe a mensagem contendo o jogador que jogará
            mensagem = recebe_mensagem()
            if mensagem.jogador == jogador:

                mensagem = joga_dados(mensagem)
                # envia a mensagem informando os pontos do jogador
                envia_mensagem(mensagem)
            else:
                envia_mensagem(mensagem)

            mensagem = recebe_mensagem()
            # sleep(1.5)

            imprime_saldo(mensagem)

            envia_mensagem(mensagem)
            
            if mensagem.saldo <= 0:
                termina_jogo(mensagem.jogador)

            mensagem = recebe_mensagem()
            
            if mensagem.jogador == jogador:
                BASTAO = True
            
            envia_mensagem(mensagem)


