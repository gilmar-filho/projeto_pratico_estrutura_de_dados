// Importação das bibliotecas necessárias, bem como do arquivo de cabeçalho data.h
#include "data.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;


CabecalhoBloco SequenceSet::lerCabecalho(fstream& arq) {
    CabecalhoBloco cabecalho;
    arq.seekg(0, ios::beg);
    arq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
    return cabecalho;
}

void SequenceSet::escreverCabecalho(fstream& arq, CabecalhoBloco& cabecalho) {
    arq.seekp(0, ios::beg);
    arq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
}

void SequenceSet::lerDadosBloco(fstream& arq, Dado* dados, int numDados) {
    arq.seekg(sizeof(CabecalhoBloco), ios::beg);
    arq.read(reinterpret_cast<char*>(dados), numDados * sizeof(Dado));
}

void SequenceSet::escreverDadosBloco(fstream& arq, Dado* dados, int numDados) {
    arq.seekp(sizeof(CabecalhoBloco), ios::beg);
    arq.write(reinterpret_cast<const char*>(dados), numDados * sizeof(Dado));
}

Dado SequenceSet::lerUltimoValor(fstream& arq, int numDados) {
    Dado ultimoDado;
    arq.seekg(sizeof(CabecalhoBloco) + (numDados - 1) * sizeof(Dado), ios::beg);
    arq.read(reinterpret_cast<char*>(&ultimoDado), sizeof(Dado));
    return ultimoDado;
}


// Construtor da classe SequenceSet
SequenceSet::SequenceSet() : numBlocos(0), disBloco(0), priSeq(0), auxDadosTam(-1) {
    criarPastaBlocos();
    auxDados = new Dado[TAM_BLOCO];
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

// Cria um novo bloco de registros
void SequenceSet::criarNovoBloco(int &indiceBloco) {
    string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(indiceBloco) + ".bin";

    ofstream blocoArq(nomeBloco, ios::binary);
    if (!blocoArq.is_open()) {
        throw runtime_error("\n ### Erro ao criar o arquivo do bloco: " + nomeBloco + "! ###");
    }

    // Inicializa o cabeçalho do bloco
    CabecalhoBloco cabecalho = {0, -1}; // Bloco vazio com próximo bloco não definido
    blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
    blocoArq.close();

    numBlocos++; // Incrementa o número total de blocos
}

// Adiciona um registro ao Sequence Set
/*void SequenceSet::adicionarRegistro(const Dado &dado) {
    //Cria o primeiro bloco
    if (priSeq >= numBlocos) criarNovoBloco(priSeq);

    // Abre o arquivo do bloco
    string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(priSeq) + ".bin";
    fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
    if (!blocoArq.is_open()) {
        throw runtime_error("\n ### Erro ao abrir o arquivo do bloco: " + nomeBloco + "! ###\n");
    }

    // Lê o cabeçalho do bloco
    CabecalhoBloco cabecalho;
    blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

    if (cabecalho.numDados == 0) {
        blocoArq.seekp(sizeof(CabecalhoBloco) + cabecalho.numDados * sizeof(Dado), ios::beg);
        blocoArq.write(reinterpret_cast<const char*>(&dado), sizeof(Dado));

        cabecalho.numDados++; // Incrementa a quantidade de registros no bloco
        
        blocoArq.seekp(0, ios::beg);
        blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
    } else if (cabecalho.numDados < TAM_BLOCO) {
        // Lê todos os dados existentes no bloco para o vetor auxDados
        blocoArq.seekg(sizeof(CabecalhoBloco), ios::beg); // Move o ponteiro para a posição após o cabeçalho
        blocoArq.read(reinterpret_cast<char*>(auxDados), cabecalho.numDados * sizeof(Dado));
        
        // Atualiza auxDadosTam para refletir o número de registros lidos
        auxDadosTam = cabecalho.numDados - 1;

        // Adiciona o novo dado ao vetor auxiliar
        auxDados[auxDadosTam++] = dado;

        if (auxDadosTam == (TAM_BLOCO-1)) { // Verifica se o vetor atingiu a capacidade máxima
            shellSort(); // Ordena o vetor auxiliar usando Shellsort

            // Reescreve os dados ordenados no bloco
            blocoArq.seekp(sizeof(CabecalhoBloco), ios::beg); // Move o ponteiro para a posição após o cabeçalho
            blocoArq.write(reinterpret_cast<const char*>(auxDados), auxDadosTam * sizeof(Dado));

            // Atualiza o cabeçalho com o novo número de registros
            cabecalho.numDados = auxDadosTam;
            blocoArq.seekp(0, ios::beg);
            blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

            // Reinicia o tamanho do vetor auxiliar
            auxDadosTam = -1;
        }
    } else {
        int proxBloco = cabecalho.proxBloco;
    
        // Lê todos os dados do bloco atual para auxDados
        blocoArq.seekg(sizeof(CabecalhoBloco), ios::beg);
        blocoArq.read(reinterpret_cast<char*>(auxDados), cabecalho.numDados * sizeof(Dado));
    
        auxDadosTam = cabecalho.numDados;
        auxDados[auxDadosTam++] = dado;  // Adiciona o novo dado ao buffer
    
        // Ordena todos os dados no buffer
        shellSort(auxDados, auxDadosTam);
    
        if (proxBloco == -1) {
            // Cria um novo bloco, pois não há próximo
            criarNovoBloco(disBloco);
            proxBloco = disBloco++;
            cabecalho.proxBloco = proxBloco;
    
            blocoArq.seekp(0, ios::beg);
            blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
        }
    
        // Abre o próximo bloco
        string nomeProxBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(proxBloco) + ".bin";
        fstream proxBlocoArq(nomeProxBloco, ios::in | ios::out | ios::binary);
        if (!proxBlocoArq.is_open()) {
            throw runtime_error("\n ### Erro ao abrir o arquivo do próximo bloco: " + nomeProxBloco + "! ###\n");
        }
    
        // Lê o cabeçalho do próximo bloco
        CabecalhoBloco cabecalhoProx;
        proxBlocoArq.read(reinterpret_cast<char*>(&cabecalhoProx), sizeof(CabecalhoBloco));
    
        if (cabecalhoProx.numDados + auxDadosTam / 2 <= TAM_BLOCO) {
            // O próximo bloco tem espaço suficiente para receber metade dos dados
            int metade = auxDadosTam / 2;
    
            // Reescreve a primeira metade no bloco atual
            cabecalho.numDados = metade;
            blocoArq.seekp(sizeof(CabecalhoBloco), ios::beg);
            blocoArq.write(reinterpret_cast<const char*>(auxDados), metade * sizeof(Dado));
            blocoArq.seekp(0, ios::beg);
            blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
    
            // Copia a segunda metade para o próximo bloco
            cabecalhoProx.numDados += auxDadosTam - metade;
            proxBlocoArq.seekp(sizeof(CabecalhoBloco) + cabecalhoProx.numDados * sizeof(Dado), ios::beg);
            proxBlocoArq.write(reinterpret_cast<const char*>(auxDados + metade), (auxDadosTam - metade) * sizeof(Dado));
            proxBlocoArq.seekp(0, ios::beg);
            proxBlocoArq.write(reinterpret_cast<char*>(&cabecalhoProx), sizeof(CabecalhoBloco));
        } else {
            // O próximo bloco está cheio, precisamos criar um terceiro bloco
            criarNovoBloco(disBloco);
            int terceiroBloco = disBloco++;
    
            // Redistribui os dados entre o próximo bloco e o terceiro bloco
            string nomeTerceiroBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(terceiroBloco) + ".bin";
            fstream terceiroBlocoArq(nomeTerceiroBloco, ios::in | ios::out | ios::binary);
            if (!terceiroBlocoArq.is_open()) {
                throw runtime_error("\n ### Erro ao abrir o arquivo do terceiro bloco: " + nomeTerceiroBloco + "! ###\n");
            }
    
            // Redistribui os dados do próximo bloco para o terceiro bloco
            Dado auxDadosProx[TAM_BLOCO];
            proxBlocoArq.seekg(sizeof(CabecalhoBloco), ios::beg);
            proxBlocoArq.read(reinterpret_cast<char*>(auxDadosProx), cabecalhoProx.numDados * sizeof(Dado));
    
            int metadeProx = cabecalhoProx.numDados / 2;
    
            // Escreve metade dos dados do próximo bloco no terceiro bloco
            CabecalhoBloco cabecalhoTerceiro;
            cabecalhoTerceiro.numDados = cabecalhoProx.numDados - metadeProx;
            terceiroBlocoArq.seekp(sizeof(CabecalhoBloco), ios::beg);
            terceiroBlocoArq.write(reinterpret_cast<const char*>(auxDadosProx + metadeProx), cabecalhoTerceiro.numDados * sizeof(Dado));
            terceiroBlocoArq.seekp(0, ios::beg);
            terceiroBlocoArq.write(reinterpret_cast<char*>(&cabecalhoTerceiro), sizeof(CabecalhoBloco));
    
            // Atualiza o próximo bloco com a metade restante
            cabecalhoProx.numDados = metadeProx;
            proxBlocoArq.seekp(sizeof(CabecalhoBloco), ios::beg);
            proxBlocoArq.write(reinterpret_cast<const char*>(auxDadosProx), metadeProx * sizeof(Dado));
            proxBlocoArq.seekp(0, ios::beg);
            proxBlocoArq.write(reinterpret_cast<char*>(&cabecalhoProx), sizeof(CabecalhoBloco));
    
            // Reescreve a primeira metade no bloco atual
            int metadeAtual = auxDadosTam / 2;
            cabecalho.numDados = metadeAtual;
            blocoArq.seekp(sizeof(CabecalhoBloco), ios::beg);
            blocoArq.write(reinterpret_cast<const char*>(auxDados), metadeAtual * sizeof(Dado));
            blocoArq.seekp(0, ios::beg);
            blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
        }
    }
    

    // Adiciona o registro ao bloco
    if (cabecalho.numDados < TAM_BLOCO) { // Verifica se há espaço no bloco
        blocoArq.seekp(sizeof(CabecalhoBloco) + cabecalho.numDados * sizeof(Dado), ios::beg);
        blocoArq.write(reinterpret_cast<const char*>(&dado), sizeof(Dado));

        cabecalho.numDados++; // Incrementa a quantidade de registros no bloco
        
        blocoArq.seekp(0, ios::beg);
        blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));
    } else {
        // Atualiza o cabeçalho para apontar para um novo bloco
        int novoBloco = disBloco + 1;
        if (novoBloco >= numBlocos) { // Cria um novo bloco se necessário
            criarNovoBloco(novoBloco);

            cabecalho.proxBloco = novoBloco;

            blocoArq.seekp(0, ios::beg);
            blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

            disBloco = novoBloco;
            adicionarRegistro(dado); // Adiciona o registro ao novo bloco
        } else { // Adiciona o registro ao próximo bloco disponível
            bool encontrou = false;
            int i = 0;
            while (i < numBlocos and !encontrou){ // Procura o próximo bloco disponível
                string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(i) + ".bin";
                fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
                if (!blocoArq.is_open()) {
                    throw runtime_error("\n ### Erro ao abrir o arquivo do bloco: " + nomeBloco + "! ###\n");
                }

                CabecalhoBloco cabecalho;
                blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

                if (cabecalho.numDados < TAM_BLOCO) {
                    disBloco = i;
                    encontrou = true;
                }

                i++;
            }

            Dado auxDado = dado; // Salva o registro para adicionar no próximo bloco disponível
            adicionarRegistro(auxDado); // Adiciona o registro ao próximo bloco disponível
        }
    }

    blocoArq.close(); // Fecha o arquivo do bloco
}*/

void SequenceSet::adicionarRegistro(Dado novoDado) {
    int blocoAtual = 0;
    bool inserido = false;

    while (!inserido) {
        // Abre o arquivo do bloco atual
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin";
        fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
        if (!blocoArq.is_open()) {
            criarNovoBloco(disBloco);
            disBloco++;
            blocoArq.open(PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin", ios::in | ios::out | ios::binary);
        }

        CabecalhoBloco cabecalho = lerCabecalho(blocoArq);
        int ultimoValor = (cabecalho.numDados > 0) ? lerUltimoValor(blocoArq, cabecalho.numDados) : INT_MIN;

        if (novoDado.valor <= ultimoValor || cabecalho.proxBloco == -1) {
            if (cabecalho.numDados < TAM_BLOCO) {
                // Inserir no bloco atual se houver espaço
                Dado auxDados[TAM_BLOCO + 1];
                lerDadosBloco(blocoArq, auxDados, cabecalho.numDados);
                auxDados[cabecalho.numDados] = novoDado;

                // Ordenar os dados e reescrever no bloco
                shellSort(auxDados, cabecalho.numDados + 1);
                cabecalho.numDados++;
                blocoArq.seekp(0, ios::beg);
                escreverCabecalho(blocoArq, cabecalho);
                escreverDadosBloco(blocoArq, auxDados, cabecalho.numDados);
                inserido = true;
            } else {
                // Bloco está cheio, criar novo bloco e redistribuir dados
                int novoBloco = criarNovoBloco(disBloco);
                Dado auxDados[TAM_BLOCO + 1];
                lerDadosBloco(blocoArq, auxDados, TAM_BLOCO);

                // Inserir o novo dado no array auxiliar
                auxDados[TAM_BLOCO] = novoDado;
                shellSort(auxDados, TAM_BLOCO + 1);

                // Transferir metade dos dados para o novo bloco
                fstream novoBlocoArq(PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(novoBloco) + ".bin", ios::in | ios::out | ios::binary);
                CabecalhoBloco cabecalhoNovo = {TAM_BLOCO / 2, -1};

                escreverDadosBloco(novoBlocoArq, auxDados + TAM_BLOCO / 2, TAM_BLOCO / 2);
                escreverCabecalho(novoBlocoArq, cabecalhoNovo);

                // Atualizar o bloco atual com os primeiros dados
                blocoArq.seekp(0, ios::beg);
                cabecalho.numDados = TAM_BLOCO / 2;
                escreverCabecalho(blocoArq, cabecalho);
                escreverDadosBloco(blocoArq, auxDados, TAM_BLOCO / 2);

                // Atualizar o próximo bloco do bloco atual
                cabecalho.proxBloco = novoBloco;
                blocoArq.seekp(0, ios::beg);
                escreverCabecalho(blocoArq, cabecalho);

                inserido = true;
            }
        } else {
            // Ir para o próximo bloco
            blocoAtual = cabecalho.proxBloco;
        }
    }
}

// Shell Sort para ordenar os registros
void SequenceSet::shellSort(Dado &auxDados, int auxDadosTam) {
    int gapsCiura[9] = {1, 4, 10, 23, 57, 132, 301, 701, 1750};
    int posicaoGap = 8;

    while (gapsCiura[posicaoGap] > auxDadosTam) {
        posicaoGap--;
    }

    Dado aux;
    int i;
    while (posicaoGap >= 0) {
        int gap = gapsCiura[posicaoGap];
        for (int j = gap; j < auxDadosTam; j++) {
            aux = auxDados[j];
            i = j;
            while (i >= gap && strcmp(auxDados[i - gap].medida, aux.medida) > 0) {
                auxDados[i] = auxDados[i - gap];
                i = i - gap;
            }
            auxDados[i] = aux;
        }
        posicaoGap--;
    }
}

// Carrega os registros de um arquivo CSV ou TXT
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

// Salva os registros em um arquivo TXT
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

//Salva os registros em um arquivo CSV
void SequenceSet::salvarEmCsv(const string &nomeArqCsv) {
    ofstream arqCsv(nomeArqCsv);
    if (!arqCsv.is_open()) {
        throw runtime_error("\n ### Erro ao criar o arquivo CSV: " + nomeArqCsv + "! ###\n");
    }

    int indiceBloco = 0; // Começa pelo primeiro bloco

    // Cabeçalho do arquivo CSV
    arqCsv << "Bloco,Registro,Medida,Quantil,Area,Sexo,Idade,Regiao,Etnia,Valor\n";

    while (indiceBloco < numBlocos) {
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(indiceBloco) + ".bin";
        ifstream blocoArq(nomeBloco, ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("\n ### Erro ao abrir o arquivo do bloco: " + nomeBloco + "! ###\n");
        }

        CabecalhoBloco cabecalho;
        blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

        for (int i = 0; i < cabecalho.numDados; i++) {
            Dado dado;
            blocoArq.read(reinterpret_cast<char*>(&dado), sizeof(Dado));
            
            // Formata os dados como linha CSV
            arqCsv << indiceBloco << "," << i + 1 << ","
                   << dado.medida << "," << dado.quantil << "," 
                   << dado.area << "," << dado.sex << "," 
                   << dado.idade << "," << dado.regiao << "," 
                   << dado.etnia << "," << dado.valor << "\n";
        }

        blocoArq.close();
        indiceBloco++;
    }

    arqCsv.close();
    cout << "\n ### Todos os registros foram salvos em " << nomeArqCsv << " no formato CSV! ###" << endl;
}

// Remove (ou não) todos os registros com as chaves informadas
void SequenceSet::removerRegistro(const string &chaveMedida, const string &chaveIdade, const string &chaveEtnia) {
    int blocoAtual = 0; // Começa pelo primeiro bloco
    int contador = 0;

    while (blocoAtual != -1) {
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin";
        fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("\n ### Erro ao abrir o arquivo do bloco: " + nomeBloco + "! ###\n");
        }

        // Lê o cabeçalho do bloco
        CabecalhoBloco cabecalho;
        blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

        // Procura pelo registro no bloco
        for (int i = 0; i < cabecalho.numDados; i++) {
            Dado dado;
            blocoArq.seekg(sizeof(CabecalhoBloco) + i * sizeof(Dado), ios::beg);
            blocoArq.read(reinterpret_cast<char*>(&dado), sizeof(Dado));

            if ((strcmp(dado.medida, chaveMedida.c_str()) == 0) &&
                (strcmp(dado.idade, chaveIdade.c_str()) == 0) &&
                (strcmp(dado.etnia, chaveEtnia.c_str())) == 0)
                contador++;
        }

        // Não encontrado neste bloco, vai para o próximo
        blocoAtual = cabecalho.proxBloco;
        blocoArq.close();
    }

    if (contador > 0){
        int cont = 0;
        cout << "\n ### " << contador << " registros foram encontrados! ###\n"
             << "\n» Deseja removê-los?: (s/n) ";
        char yesNo;
        cin >> yesNo;
        switch (yesNo){
        case 's': {
            bool todosRemovidos = false;
            while (!todosRemovidos) {
                blocoAtual = 0; // Começa pelo primeiro bloco
                while (blocoAtual != -1) {
                    string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin";
                    fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
                    if (!blocoArq.is_open()) {
                        throw runtime_error("\n ### Erro ao abrir o arquivo do bloco: " + nomeBloco + "! ###\n");
                    }

                    // Lê o cabeçalho do bloco
                    CabecalhoBloco cabecalho;
                    blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

                    //int contador = 0;
                    // Procura pelo registro no bloco
                    for (int i = 0; i < cabecalho.numDados; i++) {
                        Dado dado;
                        blocoArq.seekg(sizeof(CabecalhoBloco) + i * sizeof(Dado), ios::beg);
                        blocoArq.read(reinterpret_cast<char*>(&dado), sizeof(Dado));

                        if ((strcmp(dado.medida, chaveMedida.c_str()) == 0) &&
                            (strcmp(dado.idade, chaveIdade.c_str()) == 0) &&
                            (strcmp(dado.etnia, chaveEtnia.c_str())) == 0) {
                            cont++;
                            // Registro encontrado. Realiza a remoção lógica.
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
                            
                            blocoArq.seekp(0, ios::beg);
                            blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

                            // Atualiza o cabeçalho geral para indicar que este bloco agora tem espaço disponível
                            disBloco = blocoAtual;
                        }
                    }

                    // Não encontrado neste bloco, vai para o próximo
                    blocoAtual = cabecalho.proxBloco;
                    blocoArq.close();
                }
                if (cont == contador) todosRemovidos = true;
            }
            
            cout << "\n ### " << cont << '/' << contador << " registros removidos! ###\n";
            break;
        }
        case 'n': {
            cout << "\n ### Operação cancelada! ###\n";
            break;
        }
        default:
            break;
        }
        
    } else {
        // Registro não encontrado
        cout << "\n ### Nenhum registro com medida '" << chaveMedida << "', idade '" << chaveIdade << "' e etnia '" << chaveEtnia << "' encontrado! ###\n";
    }    
}

// Remove um registro específico, indicado por todos os campos do registro
void SequenceSet::removerRegistro(const string &med, const string &quant, const string &ar, const string &sx, const string &idd, const string &reg, const string &etn) {
    int blocoAtual = 0; // Começa pelo primeiro bloco
    bool encontrado = false;

    while (blocoAtual != -1) {
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin";
        fstream blocoArq(nomeBloco, ios::in | ios::out | ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("\n ### Erro ao abrir o arquivo do bloco: " + nomeBloco + "! ###\n");
        }

        // Lê o cabeçalho do bloco
        CabecalhoBloco cabecalho;
        blocoArq.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

        // Procura pelo registro no bloco
        for (int i = 0; i < cabecalho.numDados; i++) {
            Dado dado;
            blocoArq.seekg(sizeof(CabecalhoBloco) + i * sizeof(Dado), ios::beg);
            blocoArq.read(reinterpret_cast<char*>(&dado), sizeof(Dado));

            if ((strcmp(dado.medida, med.c_str()) == 0) &&
                (strcmp(dado.quantil, quant.c_str()) == 0) &&
                (strcmp(dado.area, ar.c_str()) == 0) &&
                (strcmp(dado.sex, sx.c_str()) == 0) &&
                (strcmp(dado.idade, idd.c_str()) == 0) &&
                (strcmp(dado.regiao, reg.c_str()) == 0) &&
                (strcmp(dado.etnia, etn.c_str()) == 0)) {
                // Registro encontrado. Realiza a remoção lógica.
                cout << "\n» Registro encontrado no bloco " << blocoAtual << "! Deseja removê-lo? (s/n): ";
                char yesNo;
                cin >> yesNo;

                switch (yesNo) {
                    case 's': {

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
                        
                        blocoArq.seekp(0, ios::beg);
                        blocoArq.write(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBloco));

                        blocoArq.close(); // Fecha o arquivo do bloco

                        // Atualiza o cabeçalho geral para indicar que este bloco agora tem espaço disponível
                        disBloco = blocoAtual;

                        cout << "\n ### Registro removido com sucesso! ###\n";
                        
                        return; // Sai após encontrar e remover o registro
                        break;
                    }

                    case 'n': {
                        cout << "\n ### Operação cancelada! ###\n";
                        break;
                    }

                    default: {
                        break;
                    }
                }
                encontrado = true;                
            }
        }

        // Não encontrado neste bloco, vai para o próximo
        blocoAtual = cabecalho.proxBloco;
        blocoArq.close();
    }

    // Registro não encontrado
    if (!encontrado) {
        cout << "\n ### Nenhum registro encontrado! ###\n";
    }
}

// Busca um registro pelo campo 'medida', 'idade' e 'etnia'
void SequenceSet::buscarRegistro(const string &chaveMedida, const string &chaveIdade, const string &chaveEtnia) {
    int blocoAtual = 0; // Começa pelo primeiro bloco
    bool encontrado = false; // Flag para indicar se o registro foi encontrado
    int contador = 0; // Contador de registros encontrados

    while (blocoAtual != -1) {
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin";
        ifstream blocoArq(nomeBloco, ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("\n ### Erro ao abrir o arquivo do bloco: " + nomeBloco + "! ###\n");
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
                
                exibirTabela(dado, encontrado); // Exibe a tabela com os registros encontrados
                encontrado = true; // Marca como encontrado
                contador++; // Conta a quantidade de registros encontrados
            }
        }

        // Não encontrado neste bloco, vai para o próximo
        blocoAtual = cabecalho.proxBloco;
        blocoArq.close();
    }

    if (encontrado) {
        cout << "└────────────────────┴─────────────────────┴─────────────────────┴─────────────────────┴─────────────────────┘\n"; 
        cout << contador << " registro(s) encontrado(s)\n"; 
    } else {
        cout << "\n ### Nenhum registro encontrado! ###\n"; // Se não encontrado
    }
}

void SequenceSet::buscarRegistro(const string &med, const string &quant, const string &ar, const string &sx, const string &idd, const string &reg, const string &etn) {
    int blocoAtual = 0; // Começa pelo primeiro bloco
    bool encontrado = false; // Flag para indicar se o registro foi encontrado

    while (blocoAtual != -1) {
        string nomeBloco = PASTA_BLOCOS + "/" + BASE_NOME_ARQ_BIN + to_string(blocoAtual) + ".bin";
        ifstream blocoArq(nomeBloco, ios::binary);
        if (!blocoArq.is_open()) {
            throw runtime_error("\n ### Erro ao abrir o arquivo do bloco: " + nomeBloco + "! ###\n");
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
            if ((strcmp(dado.medida, med.c_str()) == 0) &&
                (strcmp(dado.quantil, quant.c_str()) == 0) &&
                (strcmp(dado.area, ar.c_str()) == 0) &&
                (strcmp(dado.sex, sx.c_str()) == 0) &&
                (strcmp(dado.idade, idd.c_str()) == 0) &&
                (strcmp(dado.regiao, reg.c_str()) == 0) &&
                (strcmp(dado.etnia, etn.c_str()) == 0)){
                cout << "\n» Registro encontrado no bloco " << blocoAtual << ":\n";
                exibirRegistro(dado); // Exibe o registro encontrado

                encontrado = true; // Marca como encontrado
            }
        }

        // Não encontrado neste bloco, vai para o próximo
        blocoAtual = cabecalho.proxBloco;
        blocoArq.close();
    }

    if (!encontrado) {
        cout << "\n ### Nenhum registro encontrado! ###\n"; // Se não encontrado
    }
}

// Insere um novo registro via entrada padrão
void SequenceSet::inserirViaEntradaPadrao() {
    cin.ignore();
    Dado dado;

    printf("\n%-14s","› Medida: ");
    cin.getline(dado.medida, sizeof(dado.medida));

    printf("%-14s","› Quantil: ");
    cin.getline(dado.quantil, sizeof(dado.quantil));

    printf("%-15s","› Área: ");
    cin.getline(dado.area, sizeof(dado.area));

    printf("%-14s","› Sexo: ");
    cin.getline(dado.sex, sizeof(dado.sex));

    printf("%-14s","› Idade: ");
    cin.getline(dado.idade, sizeof(dado.idade));

    printf("%-15s","› Região: ");
    cin.getline(dado.regiao, sizeof(dado.regiao));

    printf("%-14s","› Etnia: ");
    cin.getline(dado.etnia, sizeof(dado.etnia));
    
    printf("%-14s","› Valor: ");
    cin >> dado.valor;

    adicionarRegistro(dado);
    cout << "\n ### Registro inserido com sucesso! ###\n";
}