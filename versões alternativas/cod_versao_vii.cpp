#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <stdexcept>

using namespace std;

const int TAM_BLOCO = 500;
const string NOME_ARQ_BIN = "blocos.bin";

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

struct Bloco {
    Dado* dados;      // Apontador para os dados
    int numDados;
    int proxBloco;

    // Construtor para alocar memória para o bloco
    Bloco() : dados(new Dado[TAM_BLOCO]), numDados(0), proxBloco(-1) {}

    // Destruidor para liberar a memória
    ~Bloco() {
        delete[] dados; // Libera a memória alocada
    }

    // Método para reiniciar os dados sem perder a alocação
    void reiniciarDados() {
        numDados = 0;
        proxBloco = -1;
    }
};

struct cabecalho {
    int numBlocos;
    int ultimoBloco;
};

class SequenceSet {
private:
    fstream arqBin;
    Bloco bloco;
    int ultimoBloco; // Variável para controlar o último bloco

public:
    SequenceSet();
    ~SequenceSet();
    void criarBloco();
    void adicionarRegistro(Dado &dado);
    void saveAllRecordsToTxt(const string& nomeArqTxt);
    void shell_sort(Dado dados[], int tamanho);
};

SequenceSet::SequenceSet() {
    arqBin.open(NOME_ARQ_BIN, ios::in | ios::out | ios::binary);
    if (!arqBin.is_open()) {
        // Caso o arquivo não exista, cria o primeiro bloco
        criarBloco();
        arqBin.open(NOME_ARQ_BIN, ios::out | ios::binary);
        arqBin.write(reinterpret_cast<char*>(&bloco), sizeof(Bloco)); // Grava o primeiro bloco
        arqBin.close();
        arqBin.open(NOME_ARQ_BIN, ios::in | ios::out | ios::binary); // Reabre o arquivo em modo de leitura e escrita
        ultimoBloco = 0; // O primeiro bloco é o bloco 0
    } else {
        ultimoBloco = 0;
        arqBin.seekg(0, ios::beg);
        // Verifica se já existem blocos gravados no arquivo
        while (arqBin.read(reinterpret_cast<char*>(&bloco), sizeof(Bloco))) {
            ultimoBloco = bloco.proxBloco;
        }
    }
}

SequenceSet::~SequenceSet() {
    if (arqBin.is_open()) arqBin.close();
}

void SequenceSet::criarBloco() {
    bloco.reiniciarDados();
}

void SequenceSet::adicionarRegistro(Dado &dado) {
    int posicao = bloco.numDados;
    bloco.dados[posicao] = dado;
    bloco.numDados++;

    if (bloco.numDados == TAM_BLOCO) {  // Quando o bloco está cheio
        shell_sort(bloco.dados, bloco.numDados);
        // Atualiza o próximo bloco do bloco atual
        if (ultimoBloco != -1) {
            arqBin.seekp(ultimoBloco * sizeof(Bloco), ios::beg);
            arqBin.write(reinterpret_cast<char*>(&bloco), sizeof(Bloco)); // Grava o bloco cheio
        }
        // Cria um novo bloco e encadeia
        Bloco novoBloco;
        novoBloco.proxBloco = -1;
        bloco.proxBloco = ultimoBloco; // O bloco atual aponta para o último bloco gravado
        ultimoBloco++;
        arqBin.write(reinterpret_cast<char*>(&novoBloco), sizeof(Bloco)); // Grava o novo bloco vazio
        criarBloco(); // Reinicia o bloco atual
    }
}

void SequenceSet::shell_sort(Dado dados[], int tamanho) {
    int gaps_ciura[9] = {1, 4, 10, 23, 57, 132, 301, 701, 1750};
    int posicao_gap = 8;

    while (gaps_ciura[posicao_gap] > tamanho) {
        posicao_gap--;
    }

    Dado aux;
    int i;
    while (posicao_gap >= 0) {
        int gap = gaps_ciura[posicao_gap];
        for (int j = gap; j < tamanho; j++) {
            aux = dados[j];
            i = j;
            while (i >= gap && strcmp(dados[i - gap].medida, aux.medida) > 0) {
                dados[i] = dados[i - gap];
                i -= gap;
            }
            dados[i] = aux;
        }
        posicao_gap--;
    }
}

void SequenceSet::saveAllRecordsToTxt(const string& nomeArqTxt) {
    ofstream arqTxt(nomeArqTxt);
    if (!arqTxt.is_open()) {
        cout << "Erro ao criar o arquivo " << nomeArqTxt << endl;
        return;
    }

    Bloco bloco;
    arqBin.seekg(0, ios::beg);

    int indiceBloco = 0;
    while (arqBin.read(reinterpret_cast<char*>(&bloco), sizeof(Bloco))) {
        arqTxt << "Bloco " << indiceBloco++ << ":\n";
        for (int i = 0; i < bloco.numDados; i++) {
            const Dado& dado = bloco.dados[i];
            arqTxt << "  Registro: " << dado.medida << ", "
                   << dado.quantil << ", " << dado.area << ", "
                   << dado.sex << ", " << dado.idade << ", "
                   << dado.regiao << ", " << dado.etnia << ", "
                   << fixed << setprecision(2) << dado.valor << "\n";
        }
        if (bloco.proxBloco == -1) break; // Último bloco
    }

    arqTxt.close();
    cout << "Todos os registros foram salvos em " << nomeArqTxt << endl;
}

void carregarCsv(const string& nomeArqCsv, SequenceSet& ss) {
    ifstream arqCsv(nomeArqCsv);
    if (!arqCsv.is_open()) {
        cout << "Erro ao abrir o arquivo " << nomeArqCsv << endl;
        return;
    }

    string linha;

    // Ignorar a primeira linha (cabeçalho)
    if (getline(arqCsv, linha)) {
        cout << "Cabeçalho ignorado: " << linha << endl;
    }

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
                case 4: strncp
