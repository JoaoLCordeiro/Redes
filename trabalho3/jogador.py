import random
import socket
import pickle
from time import sleep
from Mensagem import Mensagem
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

def estado_jogando():
    pass


def resultado(combinacao, dados):

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

    print("Você vai jogar!!!", end="\n")
    sleep(1)
    print("Rodando os dados ...", end='\n')
    print("Resultado:", end=" ")

    for i in range(0, 5):
        numero = random.randint(1, 6)
        print(f"[{numero}]", end=" ")
        msg.dados.append(numero)

    # faz uma vetor que guarda se aquele dado já foi travado
    travados = list([False, False, False, False, False])

    resposta = input("Você quer travar algum dos dados? 'Y' para sim, 'N' para não\n")
    contador = 1
    while resposta.upper() == "Y" and contador <= 2:

        linha = input("Quais dados você deseja travar?\n").split()
        print(linha)

        for dado in linha:
            travados[int(dado)-1] = True

        for i in range(0, 5):
            if (not travados[i]):
                msg.dados[i] = random.randint(1,6)

        print("Resultado: ", end="\n")
        for i in range(0, 5):
            print(f"[{msg.dados[i]}]", end=" ")
        print()

        if contador != 2:
            resposta = input("Você quer travar algum dos dados? 'Y' para sim, 'N' para não\n")
        contador += 1

    global FICHAS
    FICHAS = FICHAS - msg.saldo + resultado(msg.combinacao, msg.dados)
    msg.saldo = FICHAS

    return msg


def devolve_porta():
    maquina = input("Qual sua máquina?\n")
    print(maquina)
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
            combinacao = input("Escolha a combinação?\n")
            mensagem = lib_m.mensagem_combinacao(combinacao,1,jogador)
            mensagem_bytes = pickle.dumps(mensagem)

            # Manda a mensagem de combinação inicial da rodada
            sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))

            # Recebe a mensagem com a maior aposta e seu jogador
            mensagem_bytes, addr = sock.recvfrom(BUFFERSIZE)
            mensagem = pickle.loads(mensagem_bytes)

            if mensagem.jogador == jogador:

                # O mesmo joga
                mensagem = joga_dados(mensagem)

                # envia a mensagem informando os pontos do jogador
                mensagem_bytes = pickle.dumps(mensagem)
                sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))

            else:

                # Repassou a mensagem
                sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))

            # recebendo a pontuação do jogador
            mensagem_bytes, addr = sock.recvfrom(BUFFERSIZE)
            mensagem = pickle.loads(mensagem_bytes)

            print(f"O jogador {mensagem.jogador} aumentou aposta para {mensagem.saldo}.", end="\n")
            print(f"Ele apostou na combinação {mensagem.combinacao} e sua jogada foi {mensagem.dados}", end="\n")
            
            # Repassando a pontuação do jogador
            sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))
            
            if mensagem.saldo <= 0:
                print(f"O jogador {mensagem.jogador} perdeu o jogo")
                print("O jogo será finalizado")
                exit(1)
            
            mensagem_bytes, addr = sock.recvfrom(BUFFERSIZE)

            mensagem = lib_m.mensagem_bastao(jogador)
            BASTAO = False
            
            
            mensagem_bytes = pickle.dumps(mensagem)
            # Manda a mensagem do bastão
            sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))
            

        else:

            # Recebe a mensagem de combinação inicial da rodada
            mensagem_bytes, addr = sock.recvfrom(BUFFERSIZE)
            mensagem = pickle.loads(mensagem_bytes)
            
            if mensagem.bastao:
                mensagem_bytes, addr = sock.recvfrom(BUFFERSIZE)
                mensagem = pickle.loads(mensagem_bytes)

            mensagem = lib_m.mensagem_aumenta_aposta(mensagem, jogador)

            mensagem_bytes = pickle.dumps(mensagem)

            # Enviando a mensagem da aposta atual mais o jogador
            sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))


            # Recebe a mensagem contendo o jogador que jogará
            mensagem_bytes, addr = sock.recvfrom(BUFFERSIZE)
            mensagem = pickle.loads(mensagem_bytes)
            if mensagem.jogador == jogador:

                mensagem = joga_dados(mensagem)

                # envia a mensagem informando os pontos do jogador
                mensagem_bytes = pickle.dumps(mensagem)
                sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))
            else:
                sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))

            mensagem_bytes, addr = sock.recvfrom(BUFFERSIZE)
            mensagem = pickle.loads(mensagem_bytes)
            sleep(1.5)

            print(f"O jogador {mensagem.jogador} aumentou aposta para {mensagem.saldo}.", end="\n")
            print(f"Ele apostou na combinação {mensagem.combinacao} e sua jogada foi {mensagem.dados}", end="\n")

            sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))
            
            if mensagem.saldo <= 0:
                print(f"O jogador {mensagem.jogador} perdeu o jogo")
                print("O jogo será finalizado")
                exit(1)

            mensagem_bytes, addr = sock.recvfrom(BUFFERSIZE)
            mensagem = pickle.loads(mensagem_bytes)
            
            if mensagem.jogador == jogador:
                BASTAO = True
            
            sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))


            # print(FICHAS)


