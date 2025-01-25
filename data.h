#ifndef FUNCAO_H
#define FUNCAO_H

#include <iostream>
#include <fstream>
#include <string>  // Adicionando a biblioteca para usar string
using namespace std;

const int TAM_BLOCO = 500;
const string BASE_NOME_ARQ_BIN = "bloco_";
const string PASTA_BLOCOS = "blocos";

struct Dado {
    char medida[10];
    char quantil[15];
    char area[55];
    char sex[25];
    char idade[25];
    char regiao[80];
    char etnia[30];
    double valor;
};

struct CabecalhoBloco {
    int numDados;    // Quantidade de registros no bloco
    int proxBloco;   // Próximo bloco da sequência (-1 se não houver)
};

class SequenceSet {
private:
    int numBlocos;   // Número total de blocos
    int disBloco;    // Próximo bloco disponível
    int contBloco;
public:
    SequenceSet();
    ~SequenceSet();
    void criarNovoBloco(int &indiceBloco);
    void adicionarRegistro(const Dado &dado);
    void carregarArquivo(const string &nomeArqCsv, bool txtOrCsv);
    void salvarEmTxt(const string &nomeArqTxt);
    void criarPastaBlocos();
    void removerRegistro(const string &chaveMedida, double chaveValor);
    void buscarRegistro(const string &chaveMedida, const string &chaveIdade, const string &chaveEtnia);
    void buscarRegistro(const string &med, const string &quant, const string &ar, const string &sx, const string &idd, const string &reg, const string &etn, double valor);
    void inserirViaEntradaPadrao();
};

void menu();

#endif