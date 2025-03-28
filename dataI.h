#ifndef FUNCAO_H
#define FUNCAO_H

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>
#include <cstring>

using namespace std;
namespace fs = std::filesystem;

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
    int numDados;
    int proxBloco;
};

class SequenceSet {
private:
    int numBlocos;
    int disBloco;
    int priSeq;
    
    CabecalhoBloco lerCabecalho(fstream &arq);
    void escreverCabecalho(fstream &arq, CabecalhoBloco &cabecalho);
    void lerDadosBloco(fstream &arq, Dado *dados, int numDados);
    void escreverDadosBloco(fstream &arq, Dado *dados, int numDados);
    void ordenarRegistros(Dado *dados, int numDados);
    int encontrarBlocoInsercao(const Dado &dado);
    int criarNovoBloco();

public:
    SequenceSet();
    ~SequenceSet();
    void adicionarRegistro(const Dado &dado);
};

#endif