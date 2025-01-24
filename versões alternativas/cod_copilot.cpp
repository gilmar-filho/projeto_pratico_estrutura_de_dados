#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;

struct Dado { //
    string medida;
    string quantil;
    string area;
    string sex;
    string idade;
    string regiao;
    string etnia;
    bool valor;

    bool operator<(const Dado& other) const {
        return medida < other.medida; // Ordenação pelo campo 'medida'
    }
};

const int TAMANHO_BLOCO = 50;

struct Bloco {
    Dado dados[TAMANHO_BLOCO];
    int quantidade;
    int proximoBloco;

    Bloco() : quantidade(0), proximoBloco(-1) {}
};

class SequenceSet {
private:
    fstream arquivo;
    string nomeArquivo;

public:
    SequenceSet(const string& nomeArquivo) : nomeArquivo(nomeArquivo) {
        arquivo.open(nomeArquivo, ios::in | ios::out | ios::binary);
        if (!arquivo) {
            arquivo.open(nomeArquivo, ios::out | ios::binary);
            arquivo.close();
            arquivo.open(nomeArquivo, ios::in | ios::out | ios::binary);
        }
    }

    ~SequenceSet() {
        arquivo.close();
    }

    void inserir(const Dado& dado) {
        Bloco bloco;
        int posicao = 0;
        bool inserido = false;

        while (lerBloco(posicao, bloco)) {
            if (bloco.quantidade < TAMANHO_BLOCO) {
                bloco.dados[bloco.quantidade++] = dado;
                sort(bloco.dados, bloco.dados + bloco.quantidade);
                escreverBloco(posicao, bloco);
                inserido = true;
                break;
            }
            posicao = bloco.proximoBloco;
        }

        if (!inserido) {
            Bloco novoBloco;
            novoBloco.dados[0] = dado;
            novoBloco.quantidade = 1;
            escreverBloco(posicao, novoBloco);
        }
    }

    bool remover(const string& medida) {
        Bloco bloco;
        int posicao = 0;

        while (lerBloco(posicao, bloco)) {
            for (int i = 0; i < bloco.quantidade; ++i) {
                if (bloco.dados[i].medida == medida) {
                    for (int j = i; j < bloco.quantidade - 1; ++j) {
                        bloco.dados[j] = bloco.dados[j + 1];
                    }
                    bloco.quantidade--;
                    escreverBloco(posicao, bloco);
                    return true;
                }
            }
            posicao = bloco.proximoBloco;
        }
        return false;
    }

    bool buscar(const string& medida, Dado& resultado) {
        Bloco bloco;
        int posicao = 0;

        while (lerBloco(posicao, bloco)) {
            for (int i = 0; i < bloco.quantidade; ++i) {
                if (bloco.dados[i].medida == medida) {
                    resultado = bloco.dados[i];
                    return true;
                }
            }
            posicao = bloco.proximoBloco;
        }
        return false;
    }

    void carregarDadosCSV(const string& nomeArquivoCSV) {
        ifstream arquivoCSV(nomeArquivoCSV);
        string linha;

        while (getline(arquivoCSV, linha)) {
            stringstream ss(linha);
            Dado dado;
            getline(ss, dado.medida, ',');
            getline(ss, dado.quantil, ',');
            getline(ss, dado.area, ',');
            getline(ss, dado.sex, ',');
            getline(ss, dado.idade, ',');
            getline(ss, dado.regiao, ',');
            getline(ss, dado.etnia, ',');
            ss >> dado.valor;
            inserir(dado);
        }
    }

private:
    bool lerBloco(int posicao, Bloco& bloco) {
        arquivo.seekg(posicao * sizeof(Bloco), ios::beg);
        arquivo.read(reinterpret_cast<char*>(&bloco), sizeof(Bloco));
        return arquivo.gcount() == sizeof(Bloco);
    }

    void escreverBloco(int posicao, const Bloco& bloco) {
        arquivo.seekp(posicao * sizeof(Bloco), ios::beg);
        arquivo.write(reinterpret_cast<const char*>(&bloco), sizeof(Bloco));
    }
};

int main() {
    SequenceSet sequenceSet("dados.dat");
    sequenceSet.carregarDadosCSV("dados.csv");

    Dado novoDado = {"medida1", "quantil1", "area1", "M", "30", "regiao1", "etnia1", true};
    sequenceSet.inserir(novoDado);

    Dado resultado;
    if (sequenceSet.buscar("medida1", resultado)) {
        cout << "Dado encontrado: " << resultado.medida << endl;
    } else {
        cout << "Dado não encontrado" << endl;
    }

    if (sequenceSet.remover("medida1")) {
        cout << "Dado removido com sucesso" << endl;
    } else {
        cout << "Dado não encontrado para remoção" << endl;
    }

    return 0;
}