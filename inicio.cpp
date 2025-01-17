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
};

SequenceSet::SequenceSet(){
    arqBin.open(nomeArqBin, ios::in | ios::out | ios::binary);
    Bloco bloco;
    bloco.numDados = 0;
    bloco.proxBloco = -1;
    if(!arqBin.is_open()){
        arqBin.open(nomeArqBin, ios::out | ios::binary);
        arqBin.close();
        arqBin.open(nomeArqBin, ios::in | ios::out | ios::binary);
    }
}

SequenceSet::~SequenceSet(){
    if (arqBin.is_open()) arqBin.close();
}

void SequenceSet::adicionarRegistro(Dado &dado){
    Bloco blocoAtual;
    int posAtual = 0;

    arqBin.seekg(0, ios::end);
    int tamArq = arqBin.tellg();
    if (tamArq == 0){
        blocoAtual = Bloco();
        posAtual = 0;
    } else {
        posAtual = (tamArq / sizeof(Bloco)) - 1;
        arqBin.seekg(posAtual * sizeof(Bloco), ios::beg);
        arqBin.read(reinterpret_cast<char*>(&blocoAtual), sizeof(Bloco));
    }

    if (blocoAtual.numDados < tamBloco){
        blocoAtual.dados[blocoAtual.numDados] = dado;
        blocoAtual.numDados++;

        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char*>(&blocoAtual), sizeof(Bloco));
    } else {
        Bloco novoBloco;
        novoBloco.numDados = 0;
        novoBloco.proxBloco = -1;

        novoBloco.dados[novoBloco.numDados] = dado;
        novoBloco.numDados++;

        blocoAtual.proxBloco = posAtual + 1;

        arqBin.seekp(posAtual * sizeof(Bloco), ios::end);
        arqBin.write(reinterpret_cast<char*>(&blocoAtual), sizeof(Bloco));
        //blocoAtual.proxBloco = tamArq / sizeof(Bloco);
        arqBin.seekp((posAtual + 1) * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char*>(&novoBloco), sizeof(Bloco));
    }
}

void SequenceSet::readAllRecords() {
        Bloco block;
        arqBin.seekg(0, std::ios::beg);

        int blockIndex = 0;
        //while (arqBin.read(reinterpret_cast<char*>(&block), sizeof(Bloco))) {
            cout << "Bloco " << blockIndex++ << ":\n";
            //for (int i = 0; i < block.numDados; ++i) {
                const Dado& record = block.dados[0];
                cout << "  Registro: " << record.medida << ", "
                          << record.quantil << ", " << record.area << ", "
                          << record.sex << ", " << record.idade << ", "
                          << record.regiao << ", " << record.etnia << ", "
                          << record.valor << "\n";
            //}
            //if (block.nextBlock == -1) break; // Último bloco
        //}
}

void carregarCsv(string nomeArqCsv, SequenceSet &ss){
    ifstream arqCsv(nomeArqCsv);
    if (!arqCsv.is_open()){
        cout << "Erro ao abrir o arquivo " << nomeArqCsv << endl;
        return;
    }

    string linha;
    //getline(arqCsv, linha);
    while(getline(arqCsv, linha)){
        Dado dado;
        /*strcpy(dado.medida, linha.substr(0, 4).c_str());
        strcpy(dado.quantil, linha.substr(5, 9).c_str());
        strcpy(dado.area, linha.substr(15, 49).c_str());*/
        int pos = 0;
        int campo = 0;

        while ((pos = linha.find(',')) != string::npos) {
            string valor = linha.substr(0, pos);
            linha.erase(0, pos + 1);

            switch (campo++) {
                case 0: strncpy(dado.medida, valor.c_str(), 5 - 1); break;
                case 1: strncpy(dado.quantil, valor.c_str(), 10 - 1); break;
                case 2: strncpy(dado.area, valor.c_str(), 50 - 1); break;
                case 3: strncpy(dado.sex, valor.c_str(), 20 - 1); break;
                case 4: strncpy(dado.idade, valor.c_str(), 20 - 1); break;
                case 5: strncpy(dado.regiao, valor.c_str(), 75 - 1); break;
                case 6: strncpy(dado.etnia, valor.c_str(), 25 - 1); break;
            }
        }
        dado.valor = stod(linha);

        ss.adicionarRegistro(dado);
    }
    arqCsv.close();
}

int main(){
    SequenceSet ss;
    carregarCsv("Subnational-period-life-tables-2017-2019-CSV.csv", ss);
    ss.readAllRecords();
    return 0;
}