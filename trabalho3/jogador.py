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
            
            
        else:
            
            # Recebe a mensagem de combinação inicial da rodada 
            mensagem_bytes, addr = sock.recvfrom(BUFFERSIZE)
            mensagem = pickle.loads(mensagem_bytes)
            
            mensagem = lib_m.mensagem_aumenta_aposta(mensagem, jogador)
            
            mensagem_bytes = pickle.dumps(mensagem) 
            sock.sendto(mensagem_bytes, (UDP_IP, UDP_PORT_OUT))
            
        exit(1)     
        
        
            
        mensagem.ficha += 1
        mensagem.mensagem = mensagem.mensagem + 'a'
        
        mensagem_string = pickle.dumps(mensagem)    
        sock.sendto(mensagem_string, (UDP_IP, UDP_PORT_OUT))

            
        data, addr = sock.recvfrom(10000)
        data_variavel = pickle.loads(data)
        print(f"Mensagem : {data_variavel.mensagem}, Ficha: {data_variavel.ficha}\n")
        sleep(3)
        
        
    print(jogador, UDP_PORT)
        