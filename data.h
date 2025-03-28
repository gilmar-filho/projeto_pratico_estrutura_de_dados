#ifndef FUNCAO_H
#define FUNCAO_H

#include <iostream>
#include <fstream> // Adicionando a biblioteca para usar arquivos
#include <string>  // Adicionando a biblioteca para usar string
using namespace std;

const int TAM_BLOCO = 500; // Tamanho máximo de registros por bloco
const string BASE_NOME_ARQ_BIN = "bloco_"; // Nome base dos arquivos binários
const string PASTA_BLOCOS = "blocos"; // Pasta para armazenar os arquivos binários

// Estrutura para armazenar os dados
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

// Estrutura do cabeçalho do bloco
struct CabecalhoBloco {
    int numDados;    // Quantidade de registros no bloco
    int proxBloco;   // Próximo bloco da sequência (-1 se não houver)
};

// Classe para manipulação do Sequence Set
class SequenceSet {
private:
    int numBlocos;   // Número total de blocos
    int disBloco;    // Próximo bloco disponível
    int priSeq;      // Primeira sequência
    //Dado auxDados[]; // Vetor auxiliar para armazenar os registros do bloco
    //int auxDadosTam; // Tamanho do vetor auxiliar

    CabecalhoBloco lerCabecalho(fstream &arq);
    void escreverCabecalho(fstream &arq, CabecalhoBloco& cabecalho);
    void lerDadosBloco(fstream &arq, Dado* dados, int numDados);
    void escreverDadosBloco(ofstream &arq, Dado* dados, int numDados);
    Dado lerUltimoValor(fstream &arq, int numDados);
    void criarPastaBlocos();
    int criarNovoBloco();
    string lerUltimaMedida(fstream &arq, int numDados);
public:
    SequenceSet();
    ~SequenceSet();
    //void criarNovoBloco(int &indiceBloco);
    void adicionarRegistro(const Dado &dado);
    void carregarArquivo(const string &nomeArqCsv, bool txtOrCsv);
    void salvarEmTxt(const string &nomeArqTxt);
    //void salvarEmCsv(const string &nomeArqCsv);
    //void removerRegistro(const string &chaveMedida, const string &chaveIdade, const string &chaveEtnia);
    //void removerRegistro(const string &med, const string &quant, const string &ar, const string &sx, const string &idd, const string &reg, const string &etn);
    //void buscarRegistro(const string &chaveMedida, const string &chaveIdade, const string &chaveEtnia);
    //void buscarRegistro(const string &med, const string &quant, const string &ar, const string &sx, const string &idd, const string &reg, const string &etn);
    //void inserirViaEntradaPadrao();
    void shellSort(Dado *dados, int n);
};

void exibirTabela(Dado dado, bool encontrou); // Função para exibir a tabela
void exibirRegistro(Dado dado); // Função para exibir um registro
void menu(); // Função para exibir o menu
void layoutInsercaoPadrao(); // Função para exibir o layout de inserção padrão
void layoutInsercaoPorTxt(); // Função para exibir o layout de inserção por arquivo texto
void layoutRemocao(); // Função para exibir o layout de remoção
void layoutBusca(); // Função para exibir o layout de busca
void layoutSalvarArquivo(int opcao); // Função para exibir o layout de salvar em txt
void layoutSair(); // Função para exibir o layout de sair
void exibirIntro(); // Função para exibir a introdução

#endif