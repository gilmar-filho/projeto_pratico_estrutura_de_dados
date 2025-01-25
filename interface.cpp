#include <iostream>
#include "data.h"
using namespace std;

void menu(){
    cout << "\nMenu:\n";
        cout << "1. Inserir registro via entrada padrão\n";
        cout << "2. Inserir registros via arquivo texto\n";
        cout << "3. Remover registro\n";
        cout << "4. Buscar registro\n";
        cout << "5. Salvar os dados atuais em um arquivo .txt\n";
        cout << "0. Sair\n";
        cout << "Escolha uma opção: ";
}