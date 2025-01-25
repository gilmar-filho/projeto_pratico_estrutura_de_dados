#ifndef FUNCAO_H
#define FUNCAO_H

#include <iostream>
#include <fstream>
#include <string>  // Adicionando a biblioteca para usar string
using namespace std;

const int TAM_BLOCO = 500;
const string ARQ_BIN = "blocos.bin";

struct Dado {
    char medida[5];
    char quantil[10];
    char area[50];
    char sex[20];
    char idade[20];
    char regiao[75];
    char etnia[25];
    double valor;
};

struct Bloco {
    Dado dados[TAM_BLOCO];
    int numDados;
    int proxBloco;
};

class SequenceSet {
private:
    fstream arqBin;
public:
    SequenceSet();
    ~SequenceSet();
    void adicionarRegistro(Dado &dado, bool teste);
    void inserirViaEntradaPadrao();
    void inserirViaArquivoTexto(const string &nomeArqTxt);
    void removerRegistro(const string &chaveMedida, const double chaveValor);
    void buscarRegistro(const string &chaveMedida, const string &chaveIdade);
    //void readAllRecords();
    void salvarDadosEmTxt(const string& nomeArqTxt);
};

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

#endif
