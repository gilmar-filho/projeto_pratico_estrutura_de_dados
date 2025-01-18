#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio> // Para a função remove()
using namespace std;

const int tamBloco = 500;         // Tamanho máximo de registros por bloco
const string nomeArqBin = "blocos.bin"; // Nome do arquivo binário

// Estrutura que representa um dado individual
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

// Estrutura que representa um bloco de dados
struct Bloco {
    Dado dados[tamBloco]; // Vetor de dados
    int numDados;         // Número de registros no bloco
    int proxBloco;        // Próximo bloco na sequência
};

// Classe SequenceSet para gerenciar o conjunto de blocos e dados
class SequenceSet {
private:
    fstream arqBin;

public:
    SequenceSet();
    ~SequenceSet();
    void adicionarRegistroOrdenado(Dado &dado);
    void readAllRecords();
    void saveAllRecordsToTxt(const string &nomeArqTxt);
    void removerArquivo();
    void inserirViaArquivoTexto(const string &nomeArqTxt);
    void inserirViaEntradaPadrao();
    Dado* buscarDado(const string &chave);
};

// Construtor: Abre/cria o arquivo binário e inicializa o primeiro bloco, se necessário
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

// Destrutor: Fecha o arquivo binário e remove-o do sistema
SequenceSet::~SequenceSet() {
    if (arqBin.is_open())
        arqBin.close();
    if (remove(nomeArqBin.c_str()) == 0) {
        cout << "Arquivo binário '" << nomeArqBin << "' foi excluído com sucesso.\n";
    } else {
        cout << "Erro ao excluir o arquivo binário '" << nomeArqBin << "'.\n";
    }
}

// Função para adicionar um registro, mantendo os blocos ordenados
void SequenceSet::adicionarRegistroOrdenado(Dado &dado) {
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
        int i = blocoAtual.numDados - 1;
        while (i >= 0 && strcmp(blocoAtual.dados[i].medida, dado.medida) > 0) {
            blocoAtual.dados[i + 1] = blocoAtual.dados[i]; // Desloca para abrir espaço
            i--;
        }
        blocoAtual.dados[i + 1] = dado;
        blocoAtual.numDados++;
        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
    } else {
        Bloco novoBloco;
        novoBloco.numDados = 0;
        novoBloco.proxBloco = -1;
        novoBloco.dados[novoBloco.numDados++] = dado;

        blocoAtual.proxBloco = posAtual + 1;
        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
        arqBin.seekp((posAtual + 1) * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&novoBloco), sizeof(Bloco));
    }
}

// Função para exibir todos os registros no terminal
void SequenceSet::readAllRecords() {
    Bloco bloco;
    arqBin.seekg(0, ios::beg);

    int indiceBloco = 0;
    while (arqBin.read(reinterpret_cast<char *>(&bloco), sizeof(Bloco))) {
        cout << "Bloco " << indiceBloco++ << ":\n";
        for (int i = 0; i < bloco.numDados; i++) {
            const Dado &dado = bloco.dados[i];
            cout << "  Registro: " << dado.medida << ", " << dado.quantil << ", " << dado.area
                 << ", " << dado.sex << ", " << dado.idade << ", " << dado.regiao << ", "
                 << dado.etnia << ", " << dado.valor << "\n";
        }
        if (bloco.proxBloco == -1)
            break; // Último bloco
    }
}

// Salva todos os registros em um arquivo de texto
void SequenceSet::saveAllRecordsToTxt(const string &nomeArqTxt) {
    ofstream arqTxt(nomeArqTxt);
    if (!arqTxt.is_open()) {
        cout << "Erro ao criar o arquivo " << nomeArqTxt << endl;
        return;
    }

    Bloco bloco;
    arqBin.seekg(0, ios::beg);

    int indiceBloco = 0;
    while (arqBin.read(reinterpret_cast<char *>(&bloco), sizeof(Bloco))) {
        arqTxt << "Bloco " << indiceBloco++ << ":\n";
        for (int i = 0; i < bloco.numDados; i++) {
            const Dado &dado = bloco.dados[i];
            arqTxt << "  Registro: " << dado.medida << ", " << dado.quantil << ", " << dado.area
                   << ", " << dado.sex << ", " << dado.idade << ", " << dado.regiao << ", "
                   << dado.etnia << ", " << dado.valor << "\n";
        }
        if (bloco.proxBloco == -1)
            break; // Último bloco
    }

    arqTxt.close();
    cout << "Todos os registros foram salvos em " << nomeArqTxt << endl;
}

// Remove o arquivo binário manualmente
void SequenceSet::removerArquivo() {
    arqBin.close();
    if (remove(nomeArqBin.c_str()) == 0) {
        cout << "Arquivo binário '" << nomeArqBin << "' foi excluído com sucesso.\n";
    } else {
        cout << "Erro ao excluir o arquivo binário.\n";
    }
}

// Insere registros a partir de um arquivo texto
void SequenceSet::inserirViaArquivoTexto(const string &nomeArqTxt) {
    ifstream arqTxt(nomeArqTxt);
    if (!arqTxt.is_open()) {
        cout << "Erro ao abrir o arquivo de texto " << nomeArqTxt << endl;
        return;
    }

    string linha;
    while (getline(arqTxt, linha)) {
        Dado dado = {};
        // Lógica para preencher o dado...
        adicionarRegistroOrdenado(dado);
    }
    arqTxt.close();
    cout << "Dados inseridos do arquivo de texto com sucesso.\n";
}

// Insere registros via entrada padrão
void SequenceSet::inserirViaEntradaPadrao() {
    Dado dado = {};
    cout << "Digite os dados (medida, quantil, area, sex, idade, regiao, etnia, valor):\n";
    cin >> dado.medida >> dado.quantil >> dado.area >> dado.sex >> dado.idade >> dado.regiao >>
        dado.etnia >> dado.valor;
    adicionarRegistroOrdenado(dado);
    cout << "Dado inserido com sucesso.\n";
}

// Busca um dado específico utilizando busca binária
Dado *SequenceSet::buscarDado(const string &chave) {
    Bloco bloco;
    arqBin.seekg(0, ios::beg);

    while (arqBin.read(reinterpret_cast<char *>(&bloco), sizeof(Bloco))) {
        int inicio = 0, fim = bloco.numDados - 1;
        while (inicio <= fim) {
            int meio = (inicio + fim) / 2;
            if (strcmp(bloco.dados[meio].medida, chave.c_str()) == 0) {
                return &bloco.dados[meio];
            }
            if (strcmp(bloco.dados[meio].medida, chave.c_str()) < 0) {
                inicio = meio + 1;
            } else {
                fim = meio - 1;
            }
        }
        if (bloco.proxBloco == -1)
            break;
    }
    return nullptr;
}

int main() {
    SequenceSet ss;

    // Exemplos de uso
    //ss.readAllRecords();
    //ss.saveAllRecordsToTxt("registros.txt");

    Dado novoDado = {"M01", "Q1", "Area X", "Masculino", "20-30", "Região Y", "Branco", 42.5};
    ss.adicionarRegistroOrdenado(novoDado);

    string chaveBusca = "M01";
    Dado *resultado = ss.buscarDado(chaveBusca);
    if (resultado) {
        cout << "Dado encontrado: " << resultado->medida << ", " << resultado->valor << "\n";
    } else {
        cout << "Dado não encontrado.\n";
    }

    ss.readAllRecords();
    ss.saveAllRecordsToTxt("registros.txt");

    return 0;
}
