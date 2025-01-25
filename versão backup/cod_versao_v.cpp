#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include "sequence_set.h"
using namespace std;

/*const int TAM_BLOCO = 500;
const string ARQ_BIN = "blocos.bin";

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
    Dado dados[TAM_BLOCO];
    int numDados;
    int proxBloco;
};

class SequenceSet {
private:
    fstream arqBin;
public:
    SequenceSet();
    ~SequenceSet();
    //void adicionarRegistro(Dado &dado);
    void adicionarRegistro(Dado &dado, bool teste);
    void inserirViaEntradaPadrao();
    void inserirViaArquivoTexto(const string &nomeArqTxt);
    void removerRegistro(const string &chaveMedida, const double chaveValor);
    void buscarRegistro(const string &chaveMedida, const string &chaveIdade);
    void readAllRecords();
    void salvarDadosEmTxt(const string& nomeArqTxt);
};*/

//Construtor do Sequence Set
SequenceSet::SequenceSet() {
    //Abre o arquivo binário
    arqBin.open(ARQ_BIN, ios::in | ios::out | ios::binary);
    //Caso o arquivo binário não exista, cria o primeiro bloco e inicializa o arquivo
    if (!arqBin.is_open()) {
        Bloco bloco;
        bloco.numDados = 0;
        bloco.proxBloco = -1;

        arqBin.open(ARQ_BIN, ios::out | ios::binary);
        arqBin.write(reinterpret_cast<char *>(&bloco), sizeof(Bloco));
        arqBin.close();
        arqBin.open(ARQ_BIN, ios::in | ios::out | ios::binary);
    }
}

//Destrutor do Sequence Set
SequenceSet::~SequenceSet() {
    if (arqBin.is_open()) arqBin.close();
    //Remove o arquivo binário criado
    if (remove(ARQ_BIN.c_str()) == 0) {
        cout << "Arquivo binário '" << ARQ_BIN << "' foi excluído com sucesso.\n";
    } else {
        cout << "Erro ao excluir o arquivo binário '" << ARQ_BIN << "'.\n";
    }
}

//Método para adicionar um registro ao arquivo binário
void SequenceSet::adicionarRegistro(Dado &dado, bool teste) {
    Bloco blocoAtual;
    int posAtual = 0;

    //A variável 'teste' verifica se está lendo o arquivo .csv ou se está inserindo um novo arquivo
    if (teste){
        //Caso esteja inserindo um novo arquivo, verifica se há espaço em blocos intermediários
        arqBin.seekg(0, ios::beg);
        while (arqBin.read(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco))) {
            //Caso houver espaço, insere o registro no bloco
            if (blocoAtual.numDados < TAM_BLOCO) {
                blocoAtual.dados[blocoAtual.numDados] = dado;
                blocoAtual.numDados++;

                arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
                arqBin.write(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
                return;
            }
            posAtual++;
            if (blocoAtual.proxBloco == -1)
                break;
        }

        // Se não encontrou espaço em blocos intermediários, adiciona ao final
        Bloco novoBloco;
        novoBloco.numDados = 0;
        novoBloco.proxBloco = -1;

        novoBloco.dados[novoBloco.numDados] = dado;
        novoBloco.numDados++;

        if (posAtual == 0 && arqBin.tellg() == 0) {
            posAtual = 0;
        } else {
            blocoAtual.proxBloco = posAtual + 1;
            arqBin.seekp((posAtual - 1) * sizeof(Bloco), ios::beg);
            arqBin.write(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
            posAtual++;
        }

        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&novoBloco), sizeof(Bloco));
    //Caso esteja lendo um arquivo .csv ou .txt, insere o registro no final
    } else {
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

        if (blocoAtual.numDados < TAM_BLOCO) {
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
}

//Método para inserir um registro via entrada padrão
void SequenceSet::inserirViaEntradaPadrao() {
    cin.ignore();
    Dado dado;
    cout << "Inserir novo registro:\n";
    cout << "Medida: ";
    cin.getline(dado.medida, sizeof(dado.medida));
    cout << "Quantil: ";
    cin.getline(dado.quantil, sizeof(dado.quantil));
    cout << "Área: ";
    cin.getline(dado.area, sizeof(dado.area));
    cout << "Sexo: ";
    cin.getline(dado.sex, sizeof(dado.sex));
    cout << "Idade: ";
    cin.getline(dado.idade, sizeof(dado.idade));
    cout << "Região: ";
    cin.getline(dado.regiao, sizeof(dado.regiao));
    cout << "Etnia: ";
    cin.getline(dado.etnia, sizeof(dado.etnia));
    cout << "Valor: ";
    cin >> dado.valor;

    adicionarRegistro(dado, true);
    cout << "Registro inserido com sucesso!\n";
}

//Método para remover um registro por meio de uma chave e um valor
void SequenceSet::removerRegistro(const string &chaveMedida, const double chaveValor) {
    Bloco bloco;
    arqBin.seekg(0, ios::beg);
    bool encontrado = false;

    while (arqBin.read(reinterpret_cast<char *>(&bloco), sizeof(Bloco))) {
        for (int i = 0; i < bloco.numDados; i++) {
            if ((strcmp(bloco.dados[i].medida, chaveMedida.c_str()) == 0) and (bloco.dados[i].valor == chaveValor)) {
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

//Método para buscar os registros por meio de uma chave e uma faixa etária 
void SequenceSet::buscarRegistro(const string &chaveMedida, const string &chaveIdade) {
    Bloco bloco;
    arqBin.seekg(0, ios::beg);
    bool termina = false;
    //bool encontrado = false;
    int cont = 0;

    //Exibe todas as informações dos registros encontrados
    while (arqBin.read(reinterpret_cast<char *>(&bloco), sizeof(Bloco)) and !termina) {
        for (int i = 0; i < bloco.numDados; i++) {
            if ((strcmp(bloco.dados[i].medida, chaveMedida.c_str()) == 0) and (strcmp(bloco.dados[i].idade, chaveIdade.c_str()) == 0)) {
            /*if ((strcmp(bloco.dados[i].medida, chaveMedida.c_str()) == 0) and (bloco.dados[i].valor == chaveIdade)) {*/
                cout << "Registro encontrado:\n";
                cout << "  Medida: " << bloco.dados[i].medida
                     << ", Quantil: " << bloco.dados[i].quantil
                     << ", Área: " << bloco.dados[i].area
                     << ", Sexo: " << bloco.dados[i].sex
                     << ", Idade: " << bloco.dados[i].idade
                     << ", Região: " << bloco.dados[i].regiao
                     << ", Etnia: " << bloco.dados[i].etnia
                     << ", Valor: " << bloco.dados[i].valor << "\n";
                //return;
                cont++;
            }

        }
        if (bloco.proxBloco == -1) termina = true;
    }
    if (cont > 0){
        cout << cont << " registros encontrados\n";
    } else {
        cout << "Registro não encontrado.\n";
    }
}

//Método para salvar todos os registros em um arquivo .txt
void SequenceSet::salvarDadosEmTxt(const string& nomeArqTxt) {
    ofstream arqTxt(nomeArqTxt);
        if (!arqTxt.is_open()) {
            cout << "Erro ao criar o arquivo " << nomeArqTxt << endl;
            return;
        }

        Bloco bloco;
        arqBin.seekg(0, ios::beg);

        int indiceBloco = 0;
        bool termina = false;
        while ((arqBin.read(reinterpret_cast<char*>(&bloco), sizeof(Bloco))) and !termina) {
            arqTxt << "Bloco " << indiceBloco++ << ":\n";
            for (int i = 0; i < bloco.numDados; i++) {
                const Dado& dado = bloco.dados[i];
                arqTxt << "  Registro: " << dado.medida << ", "
                    << dado.quantil << ", " << dado.area << ", "
                    << dado.sex << ", " << dado.idade << ", "
                    << dado.regiao << ", " << dado.etnia << ", "
                    << dado.valor << "\n";
            }
            if (bloco.proxBloco == -1) termina = true; // Último bloco
        }

        arqTxt.close();
        cout << "Todos os registros foram salvos em " << nomeArqTxt << endl;
}

//Método para carregar um arquivo .csv ou .txt para o arquivo binário
void carregarArquivo(const string& nomeArqCsv, SequenceSet& ss, bool txtOrCsv) {
    ifstream arqCsv(nomeArqCsv);
    if (!arqCsv.is_open()) {
        cout << "Erro ao abrir o arquivo " << nomeArqCsv << endl;
        return;
    }

    string linha;

    // Ignorar a primeira linha (cabeçalho) caso o arquivo for .csv
    if (!txtOrCsv) getline(arqCsv, linha);

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

        if (txtOrCsv)
            ss.adicionarRegistro(dado, true);
        else
            ss.adicionarRegistro(dado, false);
    }

    arqCsv.close();
}

int main() {
    SequenceSet ss;
    carregarArquivo("Subnational-period-life-tables-2017-2019-CSV.csv", ss, false);
    int opcao;
    do {
        /*cout << "\nMenu:\n";
        cout << "1. Inserir registro via entrada padrão\n";
        cout << "2. Inserir registros via arquivo texto\n";
        cout << "3. Remover registro\n";
        cout << "4. Buscar registro\n";
        cout << "5. Salvar os dados atuais em um arquivo .txt\n";
        cout << "0. Sair\n";
        cout << "Escolha uma opção: ";*/
        menu();
        cin >> opcao;

        switch (opcao) {
        case 1:
            ss.inserirViaEntradaPadrao();
            break;
        case 2: {
            string nomeArqTxt;
            cout << "Informe o nome do arquivo texto: ";
            cin >> nomeArqTxt;
            carregarArquivo(nomeArqTxt, ss,true);
            break;
        }
        case 3: {
            string chaveMedida; 
            double chaveValor;
            cout << "Informe a medida para remover: ";
            cin >> chaveMedida;
            cout << "Informe o valor para remover: ";
            cin >> chaveValor;
            ss.removerRegistro(chaveMedida, chaveValor);
            break;
        }
        case 4: {
            string chaveMedida, chaveIdade;
            cout << "Informe a medida para buscar: ";
            cin >> chaveMedida;
            cin.ignore();
            cout << "Informe a idade para buscar: ";
            getline(cin, chaveIdade);

            ss.buscarRegistro(chaveMedida, chaveIdade);
            break;
        }
        case 5: {
            string nomeArqTxt;
            cout << "Informe o nome do arquivo texto: ";
            cin >> nomeArqTxt;
            ss.salvarDadosEmTxt(nomeArqTxt);
            break;
        }
        case 0:
            ss.salvarDadosEmTxt("registros.txt");
            cout << "Saindo...\n";
            break;
        default:
            cout << "Opção inválida!\n";
        }
    } while (opcao != 0);

    return 0;
}