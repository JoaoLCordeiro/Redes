#include <stdio.h>
#include "ConexaoRawSocket.h"

int main(){
    for (int i = 0 ; i < 50 ; i++){
        int mensagem = ConexaoRawSocket ("eth");
        printf ("%d\n", mensagem);
    }
}