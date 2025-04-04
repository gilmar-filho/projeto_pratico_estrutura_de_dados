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
    int contBloco;
public:
    SequenceSet();
    void criarNovoBloco(int &indiceBloco);
    void adicionarRegistro(const Dado &dado);
    void carregarCsv(const string &nomeArqCsv);
    void saveAllRecordsToTxt(const string &nomeArqTxt);
    void criarPastaBlocos();
    void removerRegistro(const string &chaveMedida, double chaveValor);
    void buscarRegistro(const string &chaveMedida, const string &chaveIdade, const string &chaveEtnia);
    void buscarRegistro(const string &med, const string &quant, const string &ar, const string &sx, const string &idd, const string &reg, const string &etn, double valor);
};

SequenceSet::SequenceSet() : numBlocos(0), priBloco(-1), disBloco(0), contBloco(0) {
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
    CabecalhoBloco cabecalho = {0, -1}; // Bloco vazio com próximo bloco não definido
    blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
    blocoArq.close();

    numBlocos++;
    //indiceBloco++;
}

void SequenceSet::adicionarRegistro(const Dado &dado) {
    //Cria o primeiro bloco
    if (disBloco >= numBlocos) {
        criarNovoBloco(disBloco);
        contBloco++;
    }

    string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(disBloco) + ".bin";
    cout << "Adicionando registro ao bloco: " << nomeBloco << endl;
    fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
    if (!blocoArq.is_open()) {
        throw runtime_error("Erro ao abrir o arquivo do bloco: " + nomeBloco);
    }

    // Lê o cabeçalho do bloco
    CabecalhoBloco cabecalho;
    blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

    // Adiciona o registro ao bloco
    if (cabecalho.numDados < TAM_BLOCO) {
        blocoArq.seekp(sizeof(CabecalhoBloco) + cabecalho.numDados * sizeof(Dado), ios::beg);
        blocoArq.write(reinterpret_cast<const char*>(&dado), sizeof(Dado));
        cabecalho.numDados++;
        cout << "numDados " << cabecalho.numDados << endl;
        blocoArq.seekp(0, ios::beg);
        blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
    } else {
        // Atualiza o cabeçalho para apontar para um novo bloco
        int novoBloco = disBloco + 1;
        if (novoBloco >= numBlocos) {
            cout << "entrou para criação de novo bloco" << endl;
            criarNovoBloco(novoBloco);
            cabecalho.proxBloco = novoBloco;
            blocoArq.seekp(0, ios::beg);
            blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
            disBloco = novoBloco;
            adicionarRegistro(dado); // Adiciona o registro ao novo bloco
        } else {
            bool encontrou = false;
            int i = 0;
            while (i < numBlocos and !encontrou){
                string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(i) + ".bin";
                fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
                if (!blocoArq.is_open()) {
                    throw runtime_error("Erro ao abrir o arquivo do bloco: " + nomeBloco);
                }

                CabecalhoBloco cabecalho;
                blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

                if (cabecalho.numDados < TAM_BLOCO) {
                    cout << "achou " << i << endl;
                    disBloco = i;
                    encontrou = true;
                }
                i++;
            }
            Dado auxDado = dado;
            adicionarRegistro(auxDado);
        }
    }

    blocoArq.close();
}

void SequenceSet::carregarCsv(const string &nomeArqCsv) {
    ifstream arqCsv(nomeArqCsv);
    if (!arqCsv.is_open()) {
        cout << "Erro ao abrir o arquivo " << nomeArqCsv << endl;
        return;
    }

    string linha;

    // Ignorar a primeira linha (cabeçalho) caso o arquivo for .csv
    //if (!txtOrCsv) 
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
            } catch (const std::invalid_argument&) {
                cout << "Erro: valor inválido no campo 'value': " << linha << endl;
                continue;
            }
        } else {
            cout << "Erro: campo 'value' vazio." << endl;
            continue;
        }

        //if (txtOrCsv)
        //    ss.adicionarRegistro(dado, true);
        //else
        //    ss.adicionarRegistro(dado, false);
        adicionarRegistro(dado);
    }

    arqCsv.close();
    cout << contBloco << " registros carregados com sucesso." << endl;
    cout << disBloco << " " << priBloco << " " << numBlocos << " " << endl;
}

void SequenceSet::saveAllRecordsToTxt(const string &nomeArqTxt) {
    ofstream arqTxt(nomeArqTxt);
    if (!arqTxt.is_open()) {
        throw runtime_error("Erro ao criar o arquivo TXT: " + nomeArqTxt);
    }

    int indiceBloco = 0;

    while (indiceBloco < numBlocos) {
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(indiceBloco) + ".bin";
        ifstream blocoArq(nomeBloco, ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("Erro ao abrir o arquivo do bloco: " + nomeBloco);
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
    cout << "Todos os registros foram salvos em " << nomeArqTxt << endl;
}

void SequenceSet::removerRegistro(const string &chaveMedida, double chaveValor) {
    int blocoAtual = 0; // Começa pelo primeiro bloco

    while (blocoAtual != -1) {
        cout << "lendo bloco " << blocoAtual << endl;
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin";
        fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("Erro ao abrir o arquivo do bloco: " + nomeBloco);
        }

        // Lê o cabeçalho do bloco
        CabecalhoBloco cabecalho;
        blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

        // Procura pelo registro no bloco
        for (int i = 0; i < cabecalho.numDados; i++) {
            Dado dado;
            blocoArq.seekg(sizeof(CabecalhoBloco) + i * sizeof(Dado), ios::beg);
            blocoArq.read(reinterpret_cast<char*>(&dado), sizeof(Dado));

            //if ((strcmp(dado.medida, chaveMedida.c_str()) == 0) and (strcmp(dado.valor, chaveValor.c_str()) == 0)) {
            if ((strcmp(dado.medida, chaveMedida.c_str()) == 0) and (dado.valor == chaveValor)) {
                // Registro encontrado. Realiza a remoção lógica.
                cout << "Registro encontrado no bloco " << blocoAtual << " e será removido.\n";

                // Sobrescreve o registro removido com o último registro do bloco
                if (i != cabecalho.numDados - 1) {
                    blocoArq.seekg(sizeof(CabecalhoBloco) + (cabecalho.numDados - 1) * sizeof(Dado), ios::beg);
                    Dado ultimoDado;
                    blocoArq.read(reinterpret_cast<char*>(&ultimoDado), sizeof(Dado));

                    blocoArq.seekp(sizeof(CabecalhoBloco) + i * sizeof(Dado), ios::beg);
                    blocoArq.write(reinterpret_cast<char*>(&ultimoDado), sizeof(Dado));
                }

                // Atualiza o cabeçalho do bloco
                cabecalho.numDados--;
                //cout << "numDados " << cabecalho.numDados << endl;
                blocoArq.seekp(0, ios::beg);
                blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

                blocoArq.close();

                // Atualiza o cabeçalho geral para indicar que este bloco agora tem espaço disponível
                disBloco = blocoAtual;

                cout << "Registro removido com sucesso.\n";
                cout << disBloco << " " << priBloco << " " << numBlocos << " " << endl;
                return; // Sai após encontrar e remover o registro
            }
        }

        // Não encontrado neste bloco, vai para o próximo
        blocoAtual = cabecalho.proxBloco;
        blocoArq.close();
    }

    cout << "Registro com medida '" << chaveMedida << "' e valor '" << chaveValor << "' não encontrado.\n";
}

void SequenceSet::buscarRegistro(const string &chaveMedida, const string &chaveIdade, const string &chaveEtnia) {
    int blocoAtual = 0; // Começa pelo primeiro bloco

    while (blocoAtual != -1) {
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin";
        ifstream blocoArq(nomeBloco, ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("Erro ao abrir o arquivo do bloco: " + nomeBloco);
        }

        // Lê o cabeçalho do bloco
        CabecalhoBloco cabecalho;
        blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

        // Procura pelo registro no bloco
        for (int i = 0; i < cabecalho.numDados; i++) {
            Dado dado;
            blocoArq.seekg(sizeof(CabecalhoBloco) + i * sizeof(Dado), ios::beg);
            blocoArq.read(reinterpret_cast<char*>(&dado), sizeof(Dado));

            // Verifica se os campos coincidem
            if (strcmp(dado.medida, chaveMedida.c_str()) == 0 &&
                strcmp(dado.idade, chaveIdade.c_str()) == 0 &&
                strcmp(dado.etnia, chaveEtnia.c_str()) == 0) {
                cout << "Registro encontrado no bloco " << blocoAtual << ":\n";
                cout << "  Medida: " << dado.medida << ", Idade: " << dado.idade
                     << ", Etnia: " << dado.etnia << ", Valor: " << dado.valor << endl;
                blocoArq.close();
                //return;
            }
        }

        // Não encontrado neste bloco, vai para o próximo
        blocoAtual = cabecalho.proxBloco;
        blocoArq.close();
    }

    //cout << "Registro não encontrado.\n";
}

void SequenceSet::buscarRegistro(const string &med, const string &quant, const string &ar, const string &sx, const string &idd, const string &reg, const string &etn, double valor) {
    int blocoAtual = 0; // Começa pelo primeiro bloco

    while (blocoAtual != -1) {
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin";
        ifstream blocoArq(nomeBloco, ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("Erro ao abrir o arquivo do bloco: " + nomeBloco);
        }

        // Lê o cabeçalho do bloco
        CabecalhoBloco cabecalho;
        blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

        // Procura pelo registro no bloco
        for (int i = 0; i < cabecalho.numDados; i++) {
            Dado dado;
            blocoArq.seekg(sizeof(CabecalhoBloco) + i * sizeof(Dado), ios::beg);
            blocoArq.read(reinterpret_cast<char*>(&dado), sizeof(Dado));

            // Verifica se os campos coincidem
            if (strcmp(dado.medida, med.c_str()) == 0 &&
                strcmp(dado.quantil, quant.c_str()) == 0 &&
                strcmp(dado.area, ar.c_str()) == 0 &&
                strcmp(dado.sex, sx.c_str()) == 0 &&
                strcmp(dado.idade, idd.c_str()) == 0 &&
                strcmp(dado.regiao, reg.c_str()) == 0 &&
                strcmp(dado.etnia, etn.c_str()) == 0 &&
                dado.valor == valor) {
                cout << "Registro encontrado no bloco " << blocoAtual << ":\n";
                cout << "  Medida: " << dado.medida << ", Idade: " << dado.idade
                     << ", Etnia: " << dado.etnia << ", Valor: " << dado.valor << endl;
                blocoArq.close();
                //return;
            }
        }

        // Não encontrado neste bloco, vai para o próximo
        blocoAtual = cabecalho.proxBloco;
        blocoArq.close();
    }

    //cout << "Registro não encontrado.\n";
}

void excluirArquivosBinarios(const std::string& pasta) {
    try {
        for (const auto& entry : fs::directory_iterator(pasta)) {
            if (fs::is_regular_file(entry) && entry.path().extension() == ".bin") {
                fs::remove(entry.path());  // Exclui o arquivo binário
                std::cout << "Arquivo excluído: " << entry.path() << std::endl;
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Erro ao acessar a pasta: " << e.what() << std::endl;
    }
}

int main(){
    SequenceSet ss;
    try {
        excluirArquivosBinarios(PASTA_BLOCOS);
        ss.carregarCsv("Subnational-period-life-tables-2017-2019-CSV.csv");
        //ss.saveAllRecordsToTxt("registros_teste.txt");
    } catch (const exception &e) {
        cerr << "Erro: " << e.what() << endl;
    }
    ss.removerRegistro("px", 0.57022);
    ss.removerRegistro("px", 0.99402);
    
    ss.adicionarRegistro({"px", "250%", "Teste", "Teste", "Teste", "Teste", "Teste", 0.123});
    ss.adicionarRegistro({"tx", "250%", "Teste", "Teste", "Teste", "Teste", "Teste", 0.456});
    ss.adicionarRegistro({"yx", "250%", "Teste", "Teste", "Teste", "Teste", "Teste", 0.789});

    //ss.buscarRegistro("px", "80-84 years", "Total");
    //ss.buscarRegistro("px", "Teste", "Teste");
    //ss.buscarRegistro("px", "2.50%", "07 Taranaki region", "Female", "80-84 years", "Regional council", "Total", 0.73948);
    ss.buscarRegistro("px", "250%", "Teste", "Teste", "Teste", "Teste", "Teste", 0.123);

    ss.saveAllRecordsToTxt("registros_teste.txt");
    
    return 0;
}