#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <algorithm> // Para std::sort
using namespace std;

const int TAM_BLOCO = 5;  // Exemplo de tamanho máximo do bloco
const string PASTA_BLOCOS = "blocos";
const string BASE_NOME_ARQ_BIN = "bloco_";

struct Dado {
    int valor;
};

struct CabecalhoBloco {
    int numDados;
    int proxBloco;
};

// Funções auxiliares
CabecalhoBloco lerCabecalho(fstream& arq) {
    CabecalhoBloco cabecalho;
    arq.seekg(0, ios::beg);
    arq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
    return cabecalho;
}

void escreverCabecalho(fstream& arq, CabecalhoBloco& cabecalho) {
    arq.seekp(0, ios::beg);
    arq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
}

void lerDadosBloco(fstream& arq, Dado* dados, int numDados) {
    arq.seekg(sizeof(CabecalhoBloco), ios::beg);
    arq.read(reinterpret_cast<char*>(dados), numDados * sizeof(Dado));
}

void escreverDadosBloco(fstream& arq, Dado* dados, int numDados) {
    arq.seekp(sizeof(CabecalhoBloco), ios::beg);
    arq.write(reinterpret_cast<const char*>(dados), numDados * sizeof(Dado));
}

int criarNovoBloco(int& disBloco) {
    int novoBloco = disBloco++;
    string nomeNovoBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(novoBloco) + ".bin";
    fstream novoBlocoArq(nomeNovoBloco, ios::out | ios::binary);
    if (!novoBlocoArq.is_open()) {
        throw runtime_error("\n ### Erro ao criar o novo bloco: " + nomeNovoBloco + "! ###\n");
    }
    CabecalhoBloco cabecalhoNovo = {0, -1};
    escreverCabecalho(novoBlocoArq, cabecalhoNovo);
    return novoBloco;
}

int lerUltimoValor(fstream& arq, int numDados) {
    Dado ultimoDado;
    arq.seekg(sizeof(CabecalhoBloco) + (numDados - 1) * sizeof(Dado), ios::beg);
    arq.read(reinterpret_cast<char*>(&ultimoDado), sizeof(Dado));
    return ultimoDado.valor;
}

void shellSort(Dado* dados, int n) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            Dado temp = dados[i];
            int j;
            for (j = i; j >= gap && dados[j - gap].valor > temp.valor; j -= gap) {
                dados[j] = dados[j - gap];
            }
            dados[j] = temp;
        }
    }
}

void adicionarRegistro(Dado novoDado, int& disBloco) {
    int blocoAtual = 0;
    bool inserido = false;

    while (!inserido) {
        // Abre o arquivo do bloco atual
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin";
        fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
        if (!blocoArq.is_open()) {
            blocoAtual = criarNovoBloco(disBloco);
            blocoArq.open(PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin", ios::in | ios::out | ios::binary);
        }

        CabecalhoBloco cabecalho = lerCabecalho(blocoArq);
        int ultimoValor = (cabecalho.numDados > 0) ? lerUltimoValor(blocoArq, cabecalho.numDados) : INT_MIN;

        if (novoDado.valor <= ultimoValor || cabecalho.proxBloco == -1) {
            if (cabecalho.numDados < TAM_BLOCO) {
                // Inserir no bloco atual se houver espaço
                Dado auxDados[TAM_BLOCO + 1];
                lerDadosBloco(blocoArq, auxDados, cabecalho.numDados);
                auxDados[cabecalho.numDados] = novoDado;

                // Ordenar os dados e reescrever no bloco
                shellSort(auxDados, cabecalho.numDados + 1);
                cabecalho.numDados++;
                blocoArq.seekp(0, ios::beg);
                escreverCabecalho(blocoArq, cabecalho);
                escreverDadosBloco(blocoArq, auxDados, cabecalho.numDados);
                inserido = true;
            } else {
                // Bloco está cheio, criar novo bloco e redistribuir dados
                int novoBloco = criarNovoBloco(disBloco);
                Dado auxDados[TAM_BLOCO + 1];
                lerDadosBloco(blocoArq, auxDados, TAM_BLOCO);

                // Inserir o novo dado no array auxiliar
                auxDados[TAM_BLOCO] = novoDado;
                shellSort(auxDados, TAM_BLOCO + 1);

                // Transferir metade dos dados para o novo bloco
                fstream novoBlocoArq(PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(novoBloco) + ".bin", ios::in | ios::out | ios::binary);
                CabecalhoBloco cabecalhoNovo = {TAM_BLOCO / 2, -1};

                escreverDadosBloco(novoBlocoArq, auxDados + TAM_BLOCO / 2, TAM_BLOCO / 2);
                escreverCabecalho(novoBlocoArq, cabecalhoNovo);

                // Atualizar o bloco atual com os primeiros dados
                blocoArq.seekp(0, ios::beg);
                cabecalho.numDados = TAM_BLOCO / 2;
                escreverCabecalho(blocoArq, cabecalho);
                escreverDadosBloco(blocoArq, auxDados, TAM_BLOCO / 2);

                // Atualizar o próximo bloco do bloco atual
                cabecalho.proxBloco = novoBloco;
                blocoArq.seekp(0, ios::beg);
                escreverCabecalho(blocoArq, cabecalho);

                inserido = true;
            }
        } else {
            // Ir para o próximo bloco
            blocoAtual = cabecalho.proxBloco;
        }
    }
}

int main() {
    int disBloco = 0;
    Dado novoDado = {10};  // Exemplo de dado a ser inserido
    adicionarRegistro(novoDado, disBloco);
    cout << "Registro inserido com sucesso!" << endl;
    return 0;
}
