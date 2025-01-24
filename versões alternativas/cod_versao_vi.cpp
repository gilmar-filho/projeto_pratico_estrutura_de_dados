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
    //void adicionarRegistro(Dado &dado);
    void adicionarRegistro(Dado &dado, bool teste);
    void inserirViaEntradaPadrao();
    void inserirViaArquivoTexto(const string &nomeArqTxt);
    void removerRegistro(const string &chaveMedida, const double chaveValor);
    void buscarRegistro(const string &chaveMedida, const string &chaveIdade);
    void readAllRecords();
    void saveAllRecordsToTxt(const string& nomeArqTxt);
    void adicionarRegistroOrdenado(Dado &dado);
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

void SequenceSet::adicionarRegistro(Dado &dado, bool teste) {
    Bloco blocoAtual;
    int posAtual = 0;

    if (teste){
        arqBin.seekg(0, ios::beg);
        while (arqBin.read(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco))) {
            if (blocoAtual.numDados < tamBloco) {
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
}

void SequenceSet::inserirViaEntradaPadrao() {
    cin.ignore();
    Dado dado;
    cout << "Inserir novo registro:\n";
    cout << "Medida: ";
    //cin.ignore();
    cin.getline(dado.medida, sizeof(dado.medida));
    cout << "Quantil: ";
    //cin.ignore();
    cin.getline(dado.quantil, sizeof(dado.quantil));
    cout << "Área: ";
    //cin.ignore();
    cin.getline(dado.area, sizeof(dado.area));
    cout << "Sexo: ";
    //cin.ignore();
    cin.getline(dado.sex, sizeof(dado.sex));
    cout << "Idade: ";
    //cin.ignore();
    cin.getline(dado.idade, sizeof(dado.idade));
    cout << "Região: ";
    //cin.ignore();
    cin.getline(dado.regiao, sizeof(dado.regiao));
    cout << "Etnia: ";
    //cin.ignore();
    cin.getline(dado.etnia, sizeof(dado.etnia));
    cout << "Valor: ";
    //cin.ignore();
    cin >> dado.valor;

    adicionarRegistro(dado, true);
    cout << "Registro inserido com sucesso!\n";
}

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

void SequenceSet::buscarRegistro(const string &chaveMedida, const string &chaveIdade) {
    Bloco bloco;
    arqBin.seekg(0, ios::beg);
    bool termina = false;
    int cont = 0;

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
    cout << cont << " registros encontrados\n";
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

/*void SequenceSet::adicionarRegistroOrdenado(Dado &dado) {
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

    // Inserção ordenada no bloco atual
    int i = blocoAtual.numDados - 1;
    while (i >= 0 && strcmp(blocoAtual.dados[i].medida, dado.medida) > 0) {
        blocoAtual.dados[i + 1] = blocoAtual.dados[i];
        i--;
    }
    blocoAtual.dados[i + 1] = dado;
    blocoAtual.numDados++;

    // Verificar se o bloco está cheio
    if (blocoAtual.numDados > tamBloco) {
        Bloco novoBloco;
        novoBloco.numDados = 0;
        novoBloco.proxBloco = -1;

        // Mover metade dos dados para o novo bloco
        int metade = tamBloco / 2;
        for (int j = metade; j < blocoAtual.numDados; j++) {
            novoBloco.dados[j - metade] = blocoAtual.dados[j];
        }
        novoBloco.numDados = blocoAtual.numDados - metade;
        blocoAtual.numDados = metade;

        // Atualizar o ponteiro do próximo bloco
        blocoAtual.proxBloco = posAtual + 1;

        // Gravar os dois blocos no arquivo
        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));

        arqBin.seekp((posAtual + 1) * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&novoBloco), sizeof(Bloco));
    } else {
        // Apenas atualizar o bloco atual
        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
    }
}*/

/*void SequenceSet::adicionarRegistroOrdenado(Dado &dado) {
    Bloco blocoAtual;
    int posAtual = 0;

    if (!arqBin.is_open()) {
        std::cerr << "Erro: O arquivo binário não está aberto." << std::endl;
        return;
    }

    arqBin.seekg(0, ios::end);
    int tamArq = arqBin.tellg();
    if (tamArq == 0) {
        blocoAtual.numDados = 0;
        blocoAtual.proxBloco = -1;
        posAtual = 0; // Posição inicial
    } else {
        posAtual = (tamArq / sizeof(Bloco)) - 1;
        if (posAtual < 0) {
            std::cerr << "Erro: Posição calculada inválida." << std::endl;
            return;
        }
        arqBin.seekg(posAtual * sizeof(Bloco), ios::beg);
        arqBin.read(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
    }

    // Inserção ordenada no bloco atual
    int i = blocoAtual.numDados - 1;
    while (i >= 0 && strcmp(blocoAtual.dados[i].medida, dado.medida) > 0) {
        if (i + 1 >= tamBloco) {
            std::cerr << "Erro: Índice fora do limite ao inserir dado." << std::endl;
            return;
        }
        blocoAtual.dados[i + 1] = blocoAtual.dados[i];
        i--;
    }
    if (i + 1 < tamBloco) {
        blocoAtual.dados[i + 1] = dado;
    } else {
        std::cerr << "Erro: Não é possível inserir o dado." << std::endl;
        return;
    }
    blocoAtual.numDados++;

    // Verificar se o bloco está cheio
    if (blocoAtual.numDados > tamBloco) {
        Bloco novoBloco;
        int meio = blocoAtual.numDados / 2;
        novoBloco.numDados = blocoAtual.numDados - meio;
        blocoAtual.numDados = meio;

        for (int j = 0; j < novoBloco.numDados; j++) {
            if (meio + j < tamBloco) {
                novoBloco.dados[j] = blocoAtual.dados[meio + j];
            } else {
                std::cerr << "Erro: Índice fora do limite ao dividir bloco." << std::endl;
                return;
            }
        }

        novoBloco.proxBloco = blocoAtual.proxBloco;
        blocoAtual.proxBloco = posAtual + 1;

        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
        arqBin.seekp(0, ios::end);
        arqBin.write(reinterpret_cast<char *>(&novoBloco), sizeof(Bloco));
    } else {
        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
    }
}*/

void SequenceSet::adicionarRegistroOrdenado(Dado &dado) {
    Bloco blocoAtual;
    int posAtual = 0;

    // Verificar se o arquivo está aberto
    if (!arqBin.is_open()) {
        std::cerr << "Erro: O arquivo binário não está aberto." << std::endl;
        return;
    }

    // Verificar o tamanho do arquivo
    arqBin.seekg(0, ios::end);
    int tamArq = arqBin.tellg();
    if (tamArq == 0) {
        // Arquivo vazio: inicializar primeiro bloco
        blocoAtual.numDados = 0;
        blocoAtual.proxBloco = -1;
        posAtual = 0; // Posição inicial
    } else {
        // Calcular posição do último bloco
        posAtual = (tamArq / sizeof(Bloco)) - 1;
        if (posAtual < 0) {
            std::cerr << "Erro: Posição inválida no arquivo binário." << std::endl;
            return;
        }
        // Ler o último bloco
        arqBin.seekg(posAtual * sizeof(Bloco), ios::beg);
        arqBin.read(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
        if (arqBin.fail()) {
            std::cerr << "Erro ao ler o bloco do arquivo." << std::endl;
            return;
        }
    }

    // Inserção ordenada no bloco atual
    int i = blocoAtual.numDados - 1;
    while (i >= 0 && strcmp(blocoAtual.dados[i].medida, dado.medida) > 0) {
        if (i + 1 >= tamBloco) {
            std::cerr << "Erro: Índice fora do limite ao inserir ordenadamente." << std::endl;
            return;
        }
        blocoAtual.dados[i + 1] = blocoAtual.dados[i];
        i--;
    }
    if (i + 1 < tamBloco) {
        blocoAtual.dados[i + 1] = dado;
        blocoAtual.numDados++;
    } else {
        std::cerr << "Erro: Não foi possível inserir o dado no bloco atual." << std::endl;
        return;
    }

    // Verificar se o bloco está cheio
    if (blocoAtual.numDados > tamBloco) {
        Bloco novoBloco;
        int meio = blocoAtual.numDados / 2;
        novoBloco.numDados = blocoAtual.numDados - meio;
        blocoAtual.numDados = meio;

        // Copiar metade dos dados para o novo bloco
        for (int j = 0; j < novoBloco.numDados; j++) {
            novoBloco.dados[j] = blocoAtual.dados[meio + j];
        }

        // Atualizar ponteiros
        novoBloco.proxBloco = blocoAtual.proxBloco;
        blocoAtual.proxBloco = posAtual + 1;

        // Escrever blocos atualizados no arquivo
        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
        if (arqBin.fail()) {
            std::cerr << "Erro ao escrever o bloco atualizado no arquivo." << std::endl;
            return;
        }
        arqBin.seekp(0, ios::end);
        arqBin.write(reinterpret_cast<char *>(&novoBloco), sizeof(Bloco));
        if (arqBin.fail()) {
            std::cerr << "Erro ao escrever o novo bloco no arquivo." << std::endl;
            return;
        }
    } else {
        // Escrever o bloco atualizado no arquivo
        arqBin.seekp(posAtual * sizeof(Bloco), ios::beg);
        arqBin.write(reinterpret_cast<char *>(&blocoAtual), sizeof(Bloco));
        if (arqBin.fail()) {
            std::cerr << "Erro ao escrever o bloco atualizado no arquivo." << std::endl;
            return;
        }
    }

    std::cout << "Registro inserido com sucesso." << std::endl;
}

/*void carregarArquivoOrdenado(const string &nomeArqCsv, SequenceSet &ss) {
    ifstream arqCsv(nomeArqCsv);
    if (!arqCsv.is_open()) {
        cout << "Erro ao abrir o arquivo " << nomeArqCsv << endl;
        return;
    }

    string linha;

    // Ignorar a primeira linha (cabeçalho)
    getline(arqCsv, linha);

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
            } catch (const std::invalid_argument &) {
                cout << "Erro: valor inválido no campo 'valor': " << linha << endl;
                continue;
            }
        } else {
            cout << "Erro: campo 'valor' vazio." << endl;
            continue;
        }

        ss.adicionarRegistroOrdenado(dado);
    }

    arqCsv.close();
}*/

void carregarArquivoOrdenado(const string &nomeArqCsv, SequenceSet &ss) {
    ifstream arqCsv(nomeArqCsv);
    if (!arqCsv.is_open()) {
        cout << "Erro ao abrir o arquivo " << nomeArqCsv << endl;
        return;
    }

    string linha;

    // Ignorar a primeira linha (cabeçalho)
    getline(arqCsv, linha);

    while (getline(arqCsv, linha)) {
        Dado dado = {};
        size_t pos = 0;
        int campo = 0;
        bool erro = false;

        // Processar cada campo da linha
        while ((pos = linha.find(',')) != string::npos) {
            string valor = linha.substr(0, pos);
            linha.erase(0, pos + 1);

            // Preencher os campos de acordo com a ordem
            switch (campo++) {
                case 0: 
                    strncpy(dado.medida, valor.c_str(), sizeof(dado.medida) - 1); 
                    dado.medida[sizeof(dado.medida) - 1] = '\0';
                    break;
                case 1: 
                    strncpy(dado.quantil, valor.c_str(), sizeof(dado.quantil) - 1); 
                    dado.quantil[sizeof(dado.quantil) - 1] = '\0';
                    break;
                case 2: 
                    strncpy(dado.area, valor.c_str(), sizeof(dado.area) - 1); 
                    dado.area[sizeof(dado.area) - 1] = '\0';
                    break;
                case 3: 
                    strncpy(dado.sex, valor.c_str(), sizeof(dado.sex) - 1); 
                    dado.sex[sizeof(dado.sex) - 1] = '\0';
                    break;
                case 4: 
                    strncpy(dado.idade, valor.c_str(), sizeof(dado.idade) - 1); 
                    dado.idade[sizeof(dado.idade) - 1] = '\0';
                    break;
                case 5: 
                    strncpy(dado.regiao, valor.c_str(), sizeof(dado.regiao) - 1); 
                    dado.regiao[sizeof(dado.regiao) - 1] = '\0';
                    break;
                case 6: 
                    strncpy(dado.etnia, valor.c_str(), sizeof(dado.etnia) - 1); 
                    dado.etnia[sizeof(dado.etnia) - 1] = '\0';
                    break;
                default:
                    cout << "Erro: Mais campos do que o esperado na linha." << endl;
                    erro = true;
                    break;
            }

            if (erro) break;
        }

        if (erro || campo < 6) {
            cout << "Erro: Linha com campos insuficientes ou inválidos." << endl;
            continue;
        }

        // Processar o campo final 'valor'
        try {
            dado.valor = std::stod(linha);
        } catch (const std::exception &) {
            cout << "Erro: Valor inválido no campo 'valor'." << endl;
            continue;
        }

        // Adicionar o registro ao SequenceSet
        ss.adicionarRegistroOrdenado(dado);
    }

    arqCsv.close();
}

void carregarArquivo(const string& nomeArqCsv, SequenceSet& ss, bool txtOrCsv) {
    ifstream arqCsv(nomeArqCsv);
    if (!arqCsv.is_open()) {
        cout << "Erro ao abrir o arquivo " << nomeArqCsv << endl;
        return;
    }

    string linha;

    // Ignorar a primeira linha (cabeçalho) caso o arquivo for .csv
    if (!txtOrCsv) {
        getline(arqCsv, linha);
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

        if (txtOrCsv)
            ss.adicionarRegistro(dado, true);
        else
            ss.adicionarRegistro(dado, false);
    }

    arqCsv.close();
}

int main() {
    SequenceSet ss;
    //carregarArquivo("Subnational-period-life-tables-2017-2019-CSV.csv", ss, false);
    carregarArquivoOrdenado("Subnational-period-life-tables-2017-2019-CSV.csv", ss);
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
            //double chaveIdade;
            cout << "Informe a medida para buscar: ";
            cin >> chaveMedida;
            cin.ignore();
            cout << "Informe a idade para buscar: ";
            getline(cin, chaveIdade);
            //cin >> chaveIdade;
            ss.buscarRegistro(chaveMedida, chaveIdade);
            break;
        }
        case 5:
            ss.readAllRecords();
            break;
        case 0:
            ss.saveAllRecordsToTxt("registros1.txt");
            cout << "Saindo...\n";
            break;
        default:
            cout << "Opção inválida!\n";
        }
    } while (opcao != 0);

    return 0;
}