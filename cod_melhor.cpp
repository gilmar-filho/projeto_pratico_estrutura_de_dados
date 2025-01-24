#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#include <filesystem>

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
    int numDados;    // Quantidade de registros no bloco
    int proxBloco;   // Próximo bloco da sequência (-1 se não houver)
};

class SequenceSet {
private:
    int numBlocos;   // Número total de blocos
    int priBloco;    // Índice do primeiro bloco
    int disBloco;    // Próximo bloco disponível

    void criarNovoBloco(int &indiceBloco);

public:
    SequenceSet();
    void adicionarRegistro(const Dado &dado);
    void carregarCsv(const string &nomeArqCsv);
    void saveAllRecordsToTxt(const string &nomeArqTxt);
    void criarPastaBlocos();
    void removerRegistro(const string &chaveMedida, double chaveValor);
};

SequenceSet::SequenceSet() : numBlocos(0), priBloco(-1), disBloco(0) {
    criarPastaBlocos();
}

void SequenceSet::criarPastaBlocos() {
    if (!fs::exists(PASTA_BLOCOS)) {
        fs::create_directory(PASTA_BLOCOS);
    }
}

void SequenceSet::criarNovoBloco(int &indiceBloco) {
    string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(indiceBloco) + ".bin";

    ofstream blocoArq(nomeBloco, ios::binary);
    if (!blocoArq.is_open()) {
        throw runtime_error("Erro ao criar o arquivo do bloco: " + nomeBloco);
    }

    // Inicializa o cabeçalho do bloco
    CabecalhoBloco cabecalho = {0, -1};
    blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
    blocoArq.close();

    numBlocos++;
}

void SequenceSet::adicionarRegistro(const Dado &dado) {
    if (priBloco == -1) {
        priBloco = disBloco;
    }

    if (disBloco >= numBlocos) {
        criarNovoBloco(disBloco);
    }

    string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(disBloco) + ".bin";
    fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
    if (!blocoArq.is_open()) {
        throw runtime_error("Erro ao abrir o arquivo do bloco: " + nomeBloco);
    }

    CabecalhoBloco cabecalho;
    blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

    if (cabecalho.numDados < TAM_BLOCO) {
        blocoArq.seekp(sizeof(CabecalhoBloco) + cabecalho.numDados * sizeof(Dado), ios::beg);
        blocoArq.write(reinterpret_cast<const char*>(&dado), sizeof(Dado));
        cabecalho.numDados++;
        blocoArq.seekp(0, ios::beg);
        blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
    } else {
        int novoBloco = disBloco + 1;
        criarNovoBloco(novoBloco);
        cabecalho.proxBloco = novoBloco;
        blocoArq.seekp(0, ios::beg);
        blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
        disBloco = novoBloco;
        adicionarRegistro(dado);
    }

    blocoArq.close();
}

void SequenceSet::removerRegistro(const string &chaveMedida, double chaveValor) {
    int blocoAtual = 0;

    while (blocoAtual != -1) {
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin";
        fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("Erro ao abrir o arquivo do bloco: " + nomeBloco);
        }

        CabecalhoBloco cabecalho;
        blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

        for (int i = 0; i < cabecalho.numDados; i++) {
            Dado dado;
            blocoArq.seekg(sizeof(CabecalhoBloco) + i * sizeof(Dado), ios::beg);
            blocoArq.read(reinterpret_cast<char*>(&dado), sizeof(Dado));

            if ((strcmp(dado.medida, chaveMedida.c_str()) == 0) && (dado.valor == chaveValor)) {
                if (i != cabecalho.numDados - 1) {
                    blocoArq.seekg(sizeof(CabecalhoBloco) + (cabecalho.numDados - 1) * sizeof(Dado), ios::beg);
                    Dado ultimoDado;
                    blocoArq.read(reinterpret_cast<char*>(&ultimoDado), sizeof(Dado));
                    blocoArq.seekp(sizeof(CabecalhoBloco) + i * sizeof(Dado), ios::beg);
                    blocoArq.write(reinterpret_cast<char*>(&ultimoDado), sizeof(Dado));
                }

                cabecalho.numDados--;
                blocoArq.seekp(0, ios::beg);
                blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

                if (cabecalho.numDados < TAM_BLOCO && disBloco > blocoAtual) {
                    disBloco = blocoAtual;
                }

                blocoArq.close();
                return;
            }
        }

        blocoAtual = cabecalho.proxBloco;
        blocoArq.close();
    }

    cout << "Registro não encontrado.\n";
}

void SequenceSet::carregarCsv(const string &nomeArqCsv) {
    ifstream arqCsv(nomeArqCsv);
    if (!arqCsv.is_open()) {
        throw runtime_error("Erro ao abrir o arquivo CSV: " + nomeArqCsv);
    }

    string linha;
    getline(arqCsv, linha); // Ignorar o cabeçalho

    while (getline(arqCsv, linha)) {
        Dado dado = {};
        size_t pos = 0;
        int campo = 0;

        while ((pos = linha.find(',')) != string::npos) {
            string valor = linha.substr(0, pos);
            linha.erase(0, pos + 1);

            switch (campo++) {
                case 0: strncpy(dado.medida, valor.c_str(), sizeof(dado.medida) - 1); break;
                case 1: strncpy(dado.quantil, valor.c_str(), sizeof(dado.quantil) - 1); break;
                case 2: strncpy(dado.area, valor.c_str(), sizeof(dado.area) - 1); break;
                case 3: strncpy(dado.sex, valor.c_str(), sizeof(dado.sex) - 1); break;
                case 4: strncpy(dado.idade, valor.c_str(), sizeof(dado.idade) - 1); break;
                case 5: strncpy(dado.regiao, valor.c_str(), sizeof(dado.regiao) - 1); break;
                case 6: strncpy(dado.etnia, valor.c_str(), sizeof(dado.etnia) - 1); break;
            }
        }

        if (!linha.empty()) {
            try {
                dado.valor = stod(linha);
            } catch (const invalid_argument &) {
                cerr << "Erro: valor inválido no campo 'valor'.\n";
                continue;
            }
        }

        adicionarRegistro(dado);
    }

    arqCsv.close();
}

void SequenceSet::saveAllRecordsToTxt(const string &nomeArqTxt) {
    ofstream arqTxt(nomeArqTxt);
    if (!arqTxt.is_open()) {
        throw runtime_error("Erro ao criar o arquivo TXT: " + nomeArqTxt);
    }

    int indiceBloco = priBloco;

    while (indiceBloco != -1) {
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(indiceBloco) + ".bin";
        ifstream blocoArq(nomeBloco, ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("Erro ao abrir o arquivo do bloco: " + nomeBloco);
        }

        CabecalhoBloco cabecalho;
        blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

        arqTxt << "Bloco " << indiceBloco << ":\n";
        for (int i = 0; i < cabecalho.numDados; i++) {
            Dado dado;
            blocoArq.read(reinterpret_cast<char*>(&dado), sizeof(Dado));
            arqTxt << "  Registro: " << dado.medida << ", " << dado.quantil << ", "
                   << dado.area << ", " << dado.sex << ", " << dado.idade << ", "
                   << dado.regiao << ", " << dado.etnia << ", " << dado.valor << "\n";
        }

        indiceBloco = cabecalho.proxBloco;
        blocoArq.close();
    }

    arqTxt.close();
}

int main() {
    SequenceSet ss;

    try {
        ss.carregarCsv("Subnational-period-life-tables-2017-2019-CSV.csv");

        ss.removerRegistro("px", 0.57022);
        ss.removerRegistro("px", 0.99402);
        
        ss.adicionarRegistro({"px", "250%", "Teste", "Teste", "Teste", "Teste", "Teste", 0.123});
        //ss.adicionarRegistro({"tx", "250%", "Teste", "Teste", "Teste", "Teste", "Teste", 0.456});
        //ss.adicionarRegistro({"yx", "250%", "Teste", "Teste", "Teste", "Teste", "Teste", 0.789});

        ss.saveAllRecordsToTxt("registros.txt");
    } catch (const exception &e) {
        cerr << "Erro: " << e.what() << endl;
    }

    return 0;
}
