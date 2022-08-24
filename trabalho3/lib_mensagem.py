from Mensagem import Mensagem
import pickle
import copy

def calcula_paridade(msg):

    #zera a paridade da mensagem ja que o calculo é feito com ela zerada
    msg_aux = copy.copy(msg)
    msg_aux.paridade = 0    

    mensagem_bytes = pickle.dumps(msg_aux)
    aux = mensagem_bytes[0]
    for i in range(1, len(mensagem_bytes)):
        aux = aux ^ mensagem_bytes[i]
    
    return aux


def mensagem_combinacao(saldo, jogador, valor_combinacao):
    
    chaves = valor_combinacao.keys()
    
    combinacao = input("Escolha a combinação?\n> ")
    while (not combinacao in chaves):
        combinacao = input("Escolha a combinação?\n> ")

    
    mensagem = Mensagem()
    
    mensagem.combinacao = valor_combinacao[combinacao][1]
    mensagem.saldo = saldo
    mensagem.jogador = jogador
    
    mensagem.paridade = calcula_paridade(mensagem)
    
    return mensagem

def mensagem_aumenta_aposta(msg, jogador_atual, valor_combinacao):

    mensagem_nova = Mensagem()
    
    print("A aposta atua é a seguinte:", end="\n")
    print(f"O jogador {msg.jogador} está apostando {msg.saldo} ficha(s) na combinação : {valor_combinacao[msg.combinacao][2]}", end='\n')
    resposta = input("Quer aumentar a aposta ? 'Y' para sim e 'N' para não\n> ")
    
    if resposta.upper() == "Y":
        
        # Usuário escolhe quanto quer aumentar na aposta
        resposta = int(input("Quanto você quer aumentar a aposta ?\n> "))
        while resposta <= 0:
            resposta = int(input("A aposta não pode ser menor que 1\n"))
        
        # Formata nova mensagem
        mensagem_nova.jogador = jogador_atual
        mensagem_nova.saldo = msg.saldo + resposta
        mensagem_nova.combinacao = msg.combinacao
        
        mensagem_nova.paridade = calcula_paridade(mensagem_nova)
        return mensagem_nova
    
    msg.paridade = calcula_paridade(msg)
    return msg
        

def mensagem_bastao(jogador):
    
    mensagem = Mensagem()
    mensagem.bastao = True
    
    mensagem.jogador = (jogador % 4) + 1
    
    mensagem.paridade = calcula_paridade(mensagem)
    return mensagem

def mensagem_erro():
    
    mensagem = Mensagem()
    mensagem.jogador = -1
    
    mensagem.paridade = calcula_paridade(mensagem)
    return mensagem
        
        
        

    
    
    
    