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
    0: 2
    ,1: 3
    ,2: 4
    ,3: 5
    ,4: 7
    ,5: 7
    ,6: 10
    ,7: 15
}

portas = {
    1 : 1307
    ,2 : 8600
    ,3 : 1456
    ,4 : 2010
}

def estado_jogando():
    pass


def joga_dados(msg):
    
    
    mensagem = Mensagem()
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
        
    print(msg.dados)

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
                pass
                # O mesmo joga
            else:
                sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))
                
        else:
            
            # Recebe a mensagem de combinação inicial da rodada 
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
                # joga
                pass
            else:
                sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))
            
        exit(1)     
        
        