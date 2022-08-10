from Mensagem import Mensagem

def mensagem_combinacao(combinacao, saldo, jogador):
    
    mensagem = Mensagem()
    
    mensagem.combinacao = combinacao
    mensagem.saldo = saldo
    mensagem.jogador = jogador
    
    return mensagem

def mensagem_aumenta_aposta(msg, jogador_atual):

    mensagem_nova = Mensagem()
    
    print("A aposta atua é a seguinte:", end="\n")
    print(f"O jogador {msg.jogador} está apostando {msg.saldo} ficha(s) na combinação : {msg.combinacao}", end='\n')
    resposta = input("Quer aumentar a aposta ? 'Y' para sim e 'N' para não\n")
    
    if resposta.upper() == "Y":
        
        # Usuário escolhe quanto quer aumentar na aposta
        resposta = int(input("Quanto você quer aumentar a aposta ?\n"))
        while resposta <= 0:
            resposta = int(input("A aposta não pode ser menor que 1\n"))
        
        # Formata nova mensagem
        mensagem_nova.jogador = jogador_atual
        mensagem_nova.saldo = msg.saldo + resposta
        mensagem_nova.combinacao = msg.combinacao
        return mensagem_nova
    
    return msg
        

def mensagem_bastao(jogador):
    
    mensagem = Mensagem()
    mensagem.bastao = True
    
    mensagem.jogador = (jogador % 4) + 1
    
    return mensagem
        
        
        

    
    
    
    