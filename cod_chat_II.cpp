#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

const int tamBloco = 500;
const string nomeArqBin = "blocos.bin";

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
    Dado dados[tamBloco];
    int numDados;
    int proxBloco;
};

class SequenceSet {
private:
    fstream arqBin;
public:
    SequenceSet();
    ~SequenceSet();
    void adicionarRegistro(Dado &dado);
    void readAllRecords();
    void saveAllRecordsToTxt(const string& nomeArqTxt);
};

SequenceSet::SequenceSet() {
    arqBin.open(nomeArqBin, ios::in | ios::out | ios::binary);
    if (!arqBin.is_open()) {
        // Se o arquivo não existir, crie-o
        arqBin.open(nomeArqBin, ios::out | ios::binary);
        Bloco bloco;
        bloco.numDados = 0;
        bloco.proxBloco = -1;
        arqBin.write(reinterpret_cast<char*>(&bloco), sizeof(Bloco));
        arqBin.close();
        arqBin.open(nomeArqBin, ios::in | ios::out | ios::binary);
    }
}

SequenceSet::~SequenceSet() {
    if (arqBin.is_open()) arqBin.close();
}

void SequenceSet::adicionarRegistro(Dado &dado) {
    Bloco blocoAtual;
    int posAtual = 0;

    // Verificar o tamanho do arquivo
    arqBin.seekg(0, ios::end);
    int tamArq = arqBin.tellg();

    if (tamArq == 0) {
        // Arquivo vazio, criar o primeiro bloco
        blocoAtual.numDados = 0;
        blocoAtual.proxBloco = -1;
    } else {
        // Ler o último bloco
        posAtual = (tamArq / sizeof(Bloco)) - 1;
        arqBin.seekg(posAtual * sizeof(Bloco), ios::beg);
        arqBin.read(reinterpret_cast<char*>(&blocoAtual), sizeof(Bloco));
    }

    if (blocoAtual.numDados < tamBloco) {
        // Adicionar registro ao bloco atual
        blocoAtual.dados[blocoAtual.numDados] = dado;
        blocoAtual.numDados++;
        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char*>(&blocoAtual), sizeof(Bloco));
    } else {
        // Criar um novo bloco
        Bloco novoBloco;
        novoBloco.numDados = 0;
        novoBloco.proxBloco = -1;
        novoBloco.dados[novoBloco.numDados] = dado;
        novoBloco.numDados++;

        // Atualizar o próximo bloco do bloco atual
        blocoAtual.proxBloco = posAtual + 1;
        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char*>(&blocoAtual), sizeof(Bloco));

        // Escrever o novo bloco no arquivo
        arqBin.seekp((posAtual + 1) * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char*>(&novoBloco), sizeof(Bloco));
    }
}

void SequenceSet::readAllRecords() {
    Bloco bloco;
    arqBin.seekg(0, ios::beg);

    int indiceBloco = 0;
    while (arqBin.read(reinterpret_cast<char*>(&bloco), sizeof(Bloco))) {
        cout << "Bloco " << indiceBloco++ << ":\n";
        for (int i = 0; i < bloco.numDados; i++) {
            const Dado& dado = bloco.dados[i];
            cout << "  Registro: " << dado.medida << ", "
                 << dado.quantil << ", " << dado.area << ", "
                 << dado.sex << ", " << dado.idade << ", "
                 << dado.regiao << ", " << dado.etnia << ", "
                 << dado.valor << "\n";
        }

        if (bloco.proxBloco == -1) break; // Último bloco
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
                   << dado.valor << "\n";
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
                case 4: strncpy(dado.idade, valor.c_str(), sizeof(dado.idade) - 1); break;
                case 5: strncpy(dado.regiao, valor.c_str(), sizeof(dado.regiao) - 1); break;
                case 6: strncpy(dado.etnia, valor.c_str(), sizeof(dado.etnia) - 1); break;
            }
        }

        if (!linha.empty()) {
            try {
                dado.valor = std::stod(linha);
            } catch (const std::invalid_argument&) {
                cout << "Erro: valor inválido no campo 'value': " << linha << endl;
                continue;
            }
        } else {
            cout << "Erro: campo 'value' vazio." << endl;
            continue;
        }

        ss.adicionarRegistro(dado);
    }

    arqCsv.close();
}

int main() {
    SequenceSet ss;
    carregarCsv("Subnational-period-life-tables-2017-2019-CSV.csv", ss);
    //ss.readAllRecords();
    ss.saveAllRecordsToTxt("registros.txt"); //Gera um arquivo txt com os registros
    return 0;
}
