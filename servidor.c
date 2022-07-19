#include <stdio.h>
#include "ConexaoRawSocket.h"

int main(){
    for (int i = 0 ; i < 1000 ; i++){
        int mensagem = ConexaoRawSocket ("lo");
        printf ("%d\n", mensagem);
    }
}