#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
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
    void inserirViaEntradaPadrao();
    void inserirViaArquivoTexto(const string &nomeArqTxt);
    void removerRegistro(const string &chaveMedida);
    void buscarRegistro(const string &chaveMedida);
    void readAllRecords();
    void saveAllRecordsToTxt(const string& nomeArqTxt) {
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
};

SequenceSet::SequenceSet() {
    arqBin.open(nomeArqBin, ios::in | ios::out | ios::binary);
    if (!arqBin.is_open()) {
        Bloco bloco;
        bloco.numDados = 0;
        bloco.proxBloco = -1;
        arqBin.open(nomeArqBin, ios::out | ios::binary);
        arqBin.write(reinterpret_cast<char *>(&bloco), sizeof(Bloco));
        arqBin.close();
        arqBin.open(nomeArqBin, ios::in | ios::out | ios::binary);
    }
}

SequenceSet::~SequenceSet() {
    if (arqBin.is_open()) arqBin.close();
    if (remove(nomeArqBin.c_str()) == 0) {
        cout << "Arquivo binário '" << nomeArqBin << "' foi excluído com sucesso.\n";
    } else {
        cout << "Erro ao excluir o arquivo binário '" << nomeArqBin << "'.\n";
    }
}

void SequenceSet::adicionarRegistro(Dado &dado) {
    Bloco blocoAtual;
    int posAtual = 0;

    arqBin.seekg(0, ios::end);
    int tamArq = arqBin.tellg();
    if (tamArq == 0) {
        blocoAtual.numDados = 0;
        blocoAtual.proxBloco = -1;
    } else {
        posAtual = (tamArq / sizeof(Bloco)) - 1;
        arqBin.seekg(posAtual * sizeof(Bloco), ios::beg);
        arqBin.read(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
    }

    if (blocoAtual.numDados < tamBloco) {
        blocoAtual.dados[blocoAtual.numDados] = dado;
        blocoAtual.numDados++;

        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
    } else {
        Bloco novoBloco;
        novoBloco.numDados = 0;
        novoBloco.proxBloco = -1;

        novoBloco.dados[novoBloco.numDados] = dado;
        novoBloco.numDados++;

        blocoAtual.proxBloco = posAtual + 1;

        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));

        arqBin.seekp((posAtual + 1) * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&novoBloco), sizeof(Bloco));
    }
}

void SequenceSet::inserirViaEntradaPadrao() {
    Dado dado;
    cout << "Inserir novo registro:\n";
    cout << "Medida: ";
    cin >> setw(sizeof(dado.medida)) >> dado.medida;
    cout << "Quantil: ";
    cin >> setw(sizeof(dado.quantil)) >> dado.quantil;
    cout << "Área: ";
    cin >> setw(sizeof(dado.area)) >> dado.area;
    cout << "Sexo: ";
    cin >> setw(sizeof(dado.sex)) >> dado.sex;
    cout << "Idade: ";
    cin >> setw(sizeof(dado.idade)) >> dado.idade;
    cout << "Região: ";
    cin >> setw(sizeof(dado.regiao)) >> dado.regiao;
    cout << "Etnia: ";
    cin >> setw(sizeof(dado.etnia)) >> dado.etnia;
    cout << "Valor: ";
    cin >> dado.valor;

    adicionarRegistro(dado);
    cout << "Registro inserido com sucesso!\n";
}

void SequenceSet::inserirViaArquivoTexto(const string &nomeArqTxt) {
    ifstream arqTxt(nomeArqTxt);
    if (!arqTxt.is_open()) {
        cout << "Erro ao abrir o arquivo " << nomeArqTxt << endl;
        return;
    }

    string linha;
    while (getline(arqTxt, linha)) {
        Dado dado = {};
        sscanf(linha.c_str(), "%4s %9s %49s %19s %19s %74s %24s %lf",
               dado.medida, dado.quantil, dado.area, dado.sex,
               dado.idade, dado.regiao, dado.etnia, &dado.valor);
        adicionarRegistro(dado);
    }
    cout << "Registros do arquivo inseridos com sucesso!\n";
}

void SequenceSet::removerRegistro(const string &chaveMedida) {
    Bloco bloco;
    arqBin.seekg(0, ios::beg);
    bool encontrado = false;

    while (arqBin.read(reinterpret_cast<char *>(&bloco), sizeof(Bloco))) {
        for (int i = 0; i < bloco.numDados; i++) {
            if (strcmp(bloco.dados[i].medida, chaveMedida.c_str()) == 0) {
                encontrado = true;
                for (int j = i; j < bloco.numDados - 1; j++) {
                    bloco.dados[j] = bloco.dados[j + 1];
                }
                bloco.numDados--;
                arqBin.seekp(-static_cast<int>(sizeof(Bloco)), ios::cur);
                arqBin.write(reinterpret_cast<char *>(&bloco), sizeof(Bloco));
                cout << "Registro removido com sucesso!\n";
                return;
            }
        }
        if (bloco.proxBloco == -1)
            break;
    }

    if (!encontrado) {
        cout << "Registro não encontrado.\n";
    }
}

void SequenceSet::buscarRegistro(const string &chaveMedida) {
    Bloco bloco;
    arqBin.seekg(0, ios::beg);

    while (arqBin.read(reinterpret_cast<char *>(&bloco), sizeof(Bloco))) {
        for (int i = 0; i < bloco.numDados; i++) {
            if (strcmp(bloco.dados[i].medida, chaveMedida.c_str()) == 0) {
                cout << "Registro encontrado:\n";
                cout << "  Medida: " << bloco.dados[i].medida
                     << ", Quantil: " << bloco.dados[i].quantil
                     << ", Área: " << bloco.dados[i].area
                     << ", Sexo: " << bloco.dados[i].sex
                     << ", Idade: " << bloco.dados[i].idade
                     << ", Região: " << bloco.dados[i].regiao
                     << ", Etnia: " << bloco.dados[i].etnia
                     << ", Valor: " << bloco.dados[i].valor << "\n";
                return;
            }
        }
        if (bloco.proxBloco == -1)
            break;
    }
    cout << "Registro não encontrado.\n";
}

void SequenceSet::readAllRecords() {
    Bloco bloco;
    arqBin.seekg(0, ios::beg);

    while (arqBin.read(reinterpret_cast<char *>(&bloco), sizeof(Bloco))) {
        for (int i = 0; i < bloco.numDados; i++) {
            cout << "  Medida: " << bloco.dados[i].medida
                 << ", Quantil: " << bloco.dados[i].quantil
                 << ", Valor: " << bloco.dados[i].valor << "\n";
        }
        if (bloco.proxBloco == -1)
            break;
    }
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
    int opcao;
    do {
        cout << "\nMenu:\n";
        cout << "1. Inserir registro via entrada padrão\n";
        cout << "2. Inserir registros via arquivo texto\n";
        cout << "3. Remover registro\n";
        cout << "4. Buscar registro\n";
        cout << "5. Listar todos os registros\n";
        cout << "0. Sair\n";
        cout << "Escolha uma opção: ";
        cin >> opcao;

        switch (opcao) {
        case 1:
            ss.inserirViaEntradaPadrao();
            break;
        case 2: {
            string nomeArqTxt;
            cout << "Informe o nome do arquivo texto: ";
            cin >> nomeArqTxt;
            ss.inserirViaArquivoTexto(nomeArqTxt);
            break;
        }
        case 3: {
            string chaveMedida;
            cout << "Informe a medida para remover: ";
            cin >> chaveMedida;
            ss.removerRegistro(chaveMedida);
            break;
        }
        case 4: {
            string chaveMedida;
            cout << "Informe a medida para buscar: ";
            cin >> chaveMedida;
            ss.buscarRegistro(chaveMedida);
            break;
        }
        case 5:
            ss.readAllRecords();
            break;
        case 0:
            ss.saveAllRecordsToTxt("registros.txt");
            cout << "Saindo...\n";
            break;
        default:
            cout << "Opção inválida!\n";
        }
    } while (opcao != 0);

    return 0;
}
