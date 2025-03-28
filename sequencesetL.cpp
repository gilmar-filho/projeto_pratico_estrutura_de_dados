#include "data.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

SequenceSet::SequenceSet() : numBlocos(0), disBloco(0), priSeq(0) {
    criarPastaBlocos();
}

// Destrutor da classe SequenceSet
SequenceSet::~SequenceSet() {
    cout << "\n» Destruindo SequenceSet...\n" << endl;
    // Limpa a pasta de blocos
    try {
        for (const auto& entry : fs::directory_iterator(PASTA_BLOCOS)) {
            if (fs::is_regular_file(entry) && entry.path().extension() == ".bin") {
                fs::remove(entry.path());  // Exclui o arquivo binário
                //std::cout << "Arquivo excluído: " << entry.path() << std::endl;
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "\n ### Erro ao acessar a pasta: " << e.what() << "! ###" << std::endl;
    }
}

// Cria a pasta para armazenar os blocos
void SequenceSet::criarPastaBlocos() {
    if (!fs::exists(PASTA_BLOCOS)) {
        fs::create_directory(PASTA_BLOCOS);
    }
}

void SequenceSet::adicionarRegistro(const Dado &novoDado) {
    int blocoAtual = priSeq;
    bool inserido = false;

    if (numBlocos == 0) criarNovoBloco();

    while (!inserido) {
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin";
        fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("\n ### Erro ao abrir o arquivo do bloco: " + nomeBloco + "! ###\n");
        }

        CabecalhoBloco cabecalho = lerCabecalho(blocoArq);
        string ultimaMedida = (cabecalho.numDados > 0) ? lerUltimaMedida(blocoArq, cabecalho.numDados) : "";

        if (strcmp(novoDado.medida, ultimaMedida.c_str()) <= 0 || cabecalho.proxBloco == -1) {
            if (cabecalho.numDados < TAM_BLOCO) {
                Dado auxDados[TAM_BLOCO + 1];
                lerDadosBloco(blocoArq, auxDados, cabecalho.numDados);
                auxDados[cabecalho.numDados] = novoDado;

                shellSort(auxDados, cabecalho.numDados + 1);
                cabecalho.numDados++;
                escreverCabecalho(blocoArq, cabecalho);
                escreverDadosBloco(blocoArq, auxDados, cabecalho.numDados);
                inserido = true;
            } else {
                int novoBloco = criarNovoBloco();
                Dado auxDados[TAM_BLOCO + 1];
                lerDadosBloco(blocoArq, auxDados, TAM_BLOCO);
                auxDados[TAM_BLOCO] = novoDado;
                shellSort(auxDados, TAM_BLOCO + 1);

                fstream novoBlocoArq(PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(novoBloco) + ".bin", ios::in | ios::out | ios::binary);
                CabecalhoBloco cabecalhoNovo = {TAM_BLOCO / 2, -1};
                escreverDadosBloco(novoBlocoArq, auxDados + TAM_BLOCO / 2, TAM_BLOCO / 2);
                escreverCabecalho(novoBlocoArq, cabecalhoNovo);

                cabecalho.numDados = TAM_BLOCO / 2;
                blocoArq.seekp(0, ios::beg);
                escreverCabecalho(blocoArq, cabecalho);
                escreverDadosBloco(blocoArq, auxDados, TAM_BLOCO / 2);

                cabecalho.proxBloco = novoBloco;
                blocoArq.seekp(0, ios::beg);
                escreverCabecalho(blocoArq, cabecalho);
                inserido = true;
            }
        } else {
            blocoAtual = cabecalho.proxBloco;
        }
    }
}

CabecalhoBloco SequenceSet::lerCabecalho(fstream &arq) {
    CabecalhoBloco cabecalho;
    arq.seekg(0, ios::beg);
    arq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
    return cabecalho;
}

void SequenceSet::escreverCabecalho(fstream &arq, CabecalhoBloco &cabecalho) {
    arq.seekp(0, ios::beg);
    arq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
}

void SequenceSet::lerDadosBloco(fstream &arq, Dado *dados, int numDados) {
    arq.seekg(sizeof(CabecalhoBloco), ios::beg);
    arq.read(reinterpret_cast<char*>(dados), numDados * sizeof(Dado));
}

void SequenceSet::escreverDadosBloco(std::ofstream &arq, Dado *dados, int numDados) {
    arq.seekp(sizeof(CabecalhoBloco), ios::beg);
    arq.write(reinterpret_cast<const char*>(dados), numDados * sizeof(Dado));
}

int SequenceSet::criarNovoBloco() {
    int novoBloco = disBloco++;
    string nomeNovoBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(novoBloco) + ".bin";
    ofstream novoBlocoArq(nomeNovoBloco, ios::binary);
    if (!novoBlocoArq.is_open()) {
        cout << "entrei aqui";
        throw runtime_error("\n ### Erro ao criar o arquivo do bloco: " + nomeNovoBloco + "! ###\n");
    }
    CabecalhoBloco cabecalhoNovo = {0, -1};
    escreverCabecalho(&novoBlocoArq, cabecalhoNovo);
    return novoBloco;
}

string SequenceSet::lerUltimaMedida(fstream &arq, int numDados) {
    Dado ultimoDado;
    arq.seekg(sizeof(CabecalhoBloco) + (numDados - 1) * sizeof(Dado), ios::beg);
    arq.read(reinterpret_cast<char*>(&ultimoDado), sizeof(Dado));
    return string(ultimoDado.medida);
}

void SequenceSet::carregarArquivo(const string &nomeArqCsv, bool txtOrCsv) {
    ifstream arqCsv(nomeArqCsv);
    if (!arqCsv.is_open()) {
        cout << "\n ### Erro ao abrir o arquivo " << nomeArqCsv << "! ###" << endl;
        return;
    }

    string linha;

    // Ignorar a primeira linha (cabeçalho) caso o arquivo for .csv
    if (!txtOrCsv) getline(arqCsv, linha);

    cout << "\n» Carregando registros..." << endl;
    while (getline(arqCsv, linha)) {
        Dado dado = {};
        size_t pos = 0;
        int campo = 0;

        while ((pos = linha.find(',')) != string::npos) { // Separa os campos da linha
            string valor = linha.substr(0, pos); // Pega o valor do campo
            linha.erase(0, pos + 1); // Remove o campo da linha

            switch (campo++) { // Preenche os campos da struct Dado
                case 0: strncpy(dado.medida, valor.c_str(), sizeof(dado.medida) - 1); break;
                case 1: strncpy(dado.quantil, valor.c_str(), sizeof(dado.quantil) - 1); break;
                case 2: strncpy(dado.area, valor.c_str(), sizeof(dado.area) - 1); break;
                case 3: strncpy(dado.sex, valor.c_str(), sizeof(dado.sex) - 1); break;
                case 4: strncpy(dado.idade, valor.c_str(), sizeof(dado.idade) - 1); break;
                case 5: strncpy(dado.regiao, valor.c_str(), sizeof(dado.regiao) - 1); break;
                case 6: strncpy(dado.etnia, valor.c_str(), sizeof(dado.etnia) - 1); break;
            }
        }

        if (!linha.empty()) { // Preenche o campo 'value'
            try {
                dado.valor = std::stod(linha); // Converte o valor para double
            } catch (const std::invalid_argument&) {
                cout << "\n ### Erro: valor inválido no campo 'value': " << linha << "! ###" <<endl;
                continue;
            }
        } else {
            cout << "\n ### Erro: campo 'value' vazio! ###" << endl;
            continue;
        }

        adicionarRegistro(dado); // Adiciona o registro ao Sequence Set
    }

    system("clear"); // Limpa a tela
    cout << "\n ### Registros carregados com sucesso! ###" << endl;

    arqCsv.close(); // Fecha o arquivo
}

void SequenceSet::salvarEmTxt(const string &nomeArqTxt) {
    ofstream arqTxt(nomeArqTxt);
    if (!arqTxt.is_open()) {
        throw runtime_error("\n ### Erro ao criar o arquivo TXT: " + nomeArqTxt + "! ###\n");
    }

    int indiceBloco = 0; // Começa pelo primeiro bloco

    while (indiceBloco < numBlocos) {
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(indiceBloco) + ".bin";
        ifstream blocoArq(nomeBloco, ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("\n ### Erro ao abrir o arquivo do bloco: " + nomeBloco + "! ###\n");
        }

        CabecalhoBloco cabecalho;
        blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

        arqTxt << "Bloco " << indiceBloco++ << ":\n";
        for (int i = 0; i < cabecalho.numDados; i++) {
            Dado dado;
            blocoArq.read(reinterpret_cast<char*>(&dado), sizeof(Dado));
            arqTxt << "  Registro: " << dado.medida << ", "
                   << dado.quantil << ", " << dado.area << ", "
                   << dado.sex << ", " << dado.idade << ", "
                   << dado.regiao << ", " << dado.etnia << ", "
                   << dado.valor << "\n";
        }

        blocoArq.close();
    }

    arqTxt.close();
    cout << "\n ### Todos os registros foram salvos em " << nomeArqTxt << "! ###" <<endl;
}

void SequenceSet::shellSort(Dado *dados, int n) {
    for (int gap = n / 2; gap > 0; gap /= 2) {
        for (int i = gap; i < n; i++) {
            Dado temp = dados[i];
            int j;
            for (j = i; j >= gap && strcmp(dados[j - gap].medida, temp.medida) > 0; j -= gap) {
                dados[j] = dados[j - gap];
            }
            dados[j] = temp;
        }
    }
}