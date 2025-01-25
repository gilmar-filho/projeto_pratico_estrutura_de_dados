#include <iostream>
#include "data.h" // Incluindo o arquivo de cabeçalho da classe SequenceSet
using namespace std;

// Função para exibir o menu
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