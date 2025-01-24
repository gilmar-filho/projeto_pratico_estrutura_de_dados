#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <algorithm>
using namespace std;

//Developed By: Gilmar Silva de Medeiros Filho, Hugo Dias Pontello e João Miguel Chaves Fernandes
//Tema: Séries Televisivas
//Turma: 14A

//Declaração do registro
struct series {
    int identificador;
    char nome[45];
    int data;
    char personagem[55];
    int visualizacao;
    float aprovacao;
    char genero[35];
};

//Função para escolha do procedimento
int procedimento(){
    int proc;
    cout << "------- Séries Televisivas -------" << endl
        << endl << "[1]Busca" << endl 
        << "[2]Inserção" << endl 
        << "[3]Impressão" << endl 
        << "[4]Ordenar os dados" << endl
        << "[5]Remover" << endl
        << "[6]Lixeira" << endl
        << endl <<"[0]Encerrar" << endl
        << endl << "Digite o número da operação: ";
    cin >> proc;
    cin.ignore();
    cout << endl << "----------------------------------" << endl << endl;
    
    return proc;
}

//Função com as perguntas utilizadas
void perguntas(int identificador){
    if (identificador == 1){
        cout << "Busca por:" << endl 
            << endl << "[N]Nome" << endl << "[G]Gênero" << endl
            << endl << "Digite 'N' ou 'G': ";
    } else if (identificador == 2){
        cout << endl << "----------------------------------" << endl << endl;
        cout << "[1]Salvar os novos dados em um arquivo .csv" << endl
            << "[2]Imprimir" << endl
            << endl << "Digite o número da operação: ";
    } else if (identificador == 3){
        cout << "[1]Visualizações na Estreia" << endl 
            << "[2]Aprovação do Público" << endl
            << endl << "Digite o número da operação: ";
    } else if (identificador == 4){
        cout << "Imprimir:" << endl << endl 
        << "[1]Todos os dados" << endl
        << "[2]Um trecho específico" << endl
        << endl << "Digite 1 ou 2: ";
    } else if (identificador == 5){
        cout << endl << "[1]Ordem Crescente" << endl
            << "[2]Ordem Decrescente" << endl
            << endl << "Digite o número da operação: ";
    } else if (identificador == 6){
        cout << "[1]Salvar os dados no mesmo arquivo .csv existente" << endl
            << "[2]Salvar em outro arquivo .csv" << endl
            << endl << "Digite o número da operação: ";
    } else if (identificador == 7){
        cout << "Deseja salvar suas alterações em um arquivo .csv?(S/n): ";
    } else if (identificador == 8){
        cout << "Todas as alterações feitas serão perdidas!!!" << endl
            << "Deseja sair sem salvar?(S/n): ";
    } else if (identificador == 9){
        cout << "[1]Restaurar apenas um dado" << endl
            << "[2]Restaurar alguns dados" << endl
            << "[3]Restaurar todos os dados" << endl
            << endl << "[0]Sair" << endl << endl
            << "Digite o número da operação: ";
    }
}

//Função para leitura dos dados do arquivo
void gravacao_em_registro(series informacoes[],ifstream& entrada1, int tamanho){
	//Processo de salvamento dos dados do arquivo em um vetor de registros
    char lixo;
    for (int i=0;i<tamanho;i++){
        informacoes[i].identificador = i + 1;
        entrada1.getline(informacoes[i].nome,45,';');
        entrada1 >> informacoes[i].data;
        entrada1 >> lixo;
        entrada1.getline(informacoes[i].personagem,55,';');
        entrada1 >> informacoes[i].visualizacao;
        entrada1 >> lixo;
        entrada1 >> informacoes[i].aprovacao;
        entrada1 >> lixo;
        entrada1.getline(informacoes[i].genero,35);
    }
}

//Função para gravar os dados em um arquivo binário
void arquivo_binario_escrita(series dados[],int tamanho){
    ofstream saida("series_binario.dat", ios::binary);
    saida.write(reinterpret_cast<const char*>(dados), sizeof(series) * tamanho);
    saida.close();
}

//Implementação do método de ordenação Shell Sort
void shell_sort(series dados[],int tamanho,int campo){
    int gaps_ciura[9] = {1,4,10,23,57,132,301,701,1750};
    int posicao_gap = 8;

    while (gaps_ciura[posicao_gap] > tamanho){
        posicao_gap--;
    }

    series aux;
    int i;
    if (campo == 0){
        //Ordenação para fazer a Busca Binária
        while (posicao_gap >= 0){
            int gap = gaps_ciura[posicao_gap];
            for (int j = gap;j < tamanho;j++){
                aux = dados[j];
                i = j;
                while (i >= gap and (strcmp(dados[i-gap].nome,aux.nome) > 0)){
                    dados[i] = dados[i-gap];
                    i = i-gap;
                }
                dados[i] = aux;
            }
            posicao_gap--;
        }
    }
    int cont = 0;
    bool parar = false;
    for (int i = (tamanho - 1); i >= 0 and !parar; i--){
        if (dados[i].identificador == -1){
            cont++;
        } else {
            parar = true;
        }
    }
    int ordem;
    if (campo == 1){
        perguntas(5);
        cin >> ordem;
        if (ordem == 1){
            //Ordem Crescente de Visualizações na Estreia
            while (posicao_gap >= 0){
                int gap = gaps_ciura[posicao_gap];
                for (int j = gap;j < (tamanho-cont);j++){
                    aux = dados[j];
                    i = j;
                    while (i >= gap and dados[i-gap].visualizacao > aux.visualizacao){
                        dados[i] = dados[i-gap];
                        i = i-gap;
                    }
                    dados[i] = aux;
                }
                posicao_gap--;
            }
            for (int j = 0; j < (tamanho-cont); j++){
                dados[j].identificador = j+1;
            }
        } else if (ordem == 2){
            //Ordem de Decrescente de Visualizações na Estreia
            while (posicao_gap >= 0){
                int gap = gaps_ciura[posicao_gap];
                for (int j = gap;j < (tamanho-cont);j++){
                    aux = dados[j];
                    i = j;
                    while (i >= gap and dados[i-gap].visualizacao < aux.visualizacao){
                        dados[i] = dados[i-gap];
                        i = i-gap;
                    }
                    dados[i] = aux;
                }
                posicao_gap--;
            }
            for (int j = 0; j < (tamanho-cont); j++){
                dados[j].identificador = j+1;
            }
        } else {
            campo = -1;
            cout << endl << "----------------------------------" << endl << endl;
            cout << setw(5) << setfill(' ') << "" << "Procedimento Inválido!" << endl << endl;
        }
    } else if (campo == 2){
        perguntas(5);
        cin >> ordem;
        if (ordem == 1){
            //Ordem Crescente de Aprovação do Público
            while (posicao_gap >= 0){
                int gap = gaps_ciura[posicao_gap];
                for (int j = gap;j < (tamanho-cont);j++){
                    aux = dados[j];
                    i = j;
                    while (i >= gap and dados[i-gap].aprovacao > aux.aprovacao){
                        dados[i] = dados[i-gap];
                        i = i-gap;
                    }
                    dados[i] = aux;
                }
                posicao_gap--;
            }
            for (int j = 0; j < (tamanho-cont); j++){
                dados[j].identificador = j+1;
            }
        } else if (ordem == 2){
            //Ordem Decrescente de Aprovação do Público
            while (posicao_gap >= 0){
                int gap = gaps_ciura[posicao_gap];
                for (int j = gap;j < (tamanho-cont);j++){
                    aux = dados[j];
                    i = j;
                    while (i >= gap and dados[i-gap].aprovacao < aux.aprovacao){
                        dados[i] = dados[i-gap];
                        i = i-gap;
                    }
                    dados[i] = aux;
                }
                posicao_gap--;
            }
            for (int j = 0; j < (tamanho-cont); j++){
                dados[j].identificador = j+1;
            }
        } else {
            campo = -1;
            cout << endl << "----------------------------------" << endl << endl;
            cout << setw(5) << setfill(' ') << "" << "Procedimento Inválido!" << endl << endl;
        }
    }
}

//Função de Busca Binária por Nome
int busca_binaria_nome(series dados[],char buscador[],int pos_inicial,int pos_final){
    int meio = (pos_inicial+pos_final)/2;
    if ((strcmp(buscador,dados[meio].nome) == 0) and dados[meio].identificador != -1){
        system("clear");
        cout << endl << "---------------------------------- Resultado: ----------------------------------" << endl << endl;
        cout << dados[meio].identificador << ": " << dados[meio].nome << "," <<
                dados[meio].data << "," <<
                dados[meio].personagem << "," <<
                dados[meio].visualizacao << "," <<
                dados[meio].aprovacao << "," <<
                dados[meio].genero << endl;
        cout << endl << "--------------------------------------------------------------------------------" << endl << endl;
        return meio;
    } else if (pos_inicial < pos_final){
        if (strcmp(buscador,dados[meio].nome) > 0){
            return busca_binaria_nome(dados,buscador,meio+1,pos_final);
        } else {
            return busca_binaria_nome(dados,buscador,pos_inicial,meio-1);
        }
    } else {
        system("clear");
        cout << endl << "---------------------------------- Resultado: ----------------------------------" << endl << endl;
        cout << setw(22) << setfill(' ') << "" << "Dado inexistente no banco de dados!" << endl;
        cout << endl << "--------------------------------------------------------------------------------" << endl << endl;
        return -1;
    }
}

//Função para Busca Sequencial por Gênero
void busca(series dados[],int tam){
    char buscador[45];
    bool achou = false;
    cout << "Gênero: ";
    cin.ignore();
    cin.getline(buscador,35);
    system("clear");
    cout << endl << "---------------------------------- Resultado: ----------------------------------" << endl << endl;
    for (int j=0;j<tam;j++){
        if ((strcmp(dados[j].genero,buscador) == 0) and dados[j].identificador != -1){
            achou = true;
            cout << dados[j].identificador << ": " << dados[j].nome << "," <<
                dados[j].data << "," <<
                dados[j].personagem << "," <<
                dados[j].visualizacao << "," <<
                dados[j].aprovacao << "," <<
                dados[j].genero << endl;
        }
    }
    if (achou == false){
        system("clear");
        cout << setw(22) << setfill(' ') << "" << "Dado inexistente no banco de dados!" << endl;
    }
    cout << endl << "--------------------------------------------------------------------------------" << endl << endl;
}

//Função para redimencionar o vetor de registros
void redimensionar(series* &dados,int &tam,int quant){
    series *temp = new series[tam+quant];
    memcpy(temp,dados,sizeof(series)*tam);
    delete[] dados;
    dados = temp;
    tam += quant;
}

//Função para deletar um dado
void remocao(series* &dados, int tam){
    char buscador[45];
    cout << "Digite o nome da série que deseja remover: ";
    cin.getline(buscador, 45);

    bool achou = false;
    int pos;
    for (int i = 0; i < tam and (achou == false); i++){
        if (strcmp(dados[i].nome,buscador) == 0 and dados[i].identificador != -1){
            achou = true;
            pos = i;
        }
    }
    int cont = 1;
    for (int i = (tam-1); i >= 0 and dados[i].identificador == -1; i--){
            cont++;
    }
    if (achou){
        if (pos != (tam - cont)){
            dados[(tam - cont)].identificador = dados[pos].identificador;
            dados[pos].identificador = -1;
            swap(dados[(tam - cont)],dados[pos]);
        } else {
            dados[pos].identificador = -1;
        }
        system("clear");
        cout << endl << "--------------------------------------------------------------------------------" << endl;
        cout << endl << setw(27) << setfill(' ') << "" << "Dado removido com sucesso!" << endl;
        cout << endl << "--------------------------------------------------------------------------------" << endl;
    } else {
        system("clear");
        cout << endl << "--------------------------------------------------------------------------------" << endl;
        cout << endl << setw(22) << setfill(' ') << "" << "Dado inexistente no banco de dados!" << endl;
        cout << endl << "--------------------------------------------------------------------------------" << endl;
    }
}

//Função para inserir dado(s) no arquivo
void insercao(series* &dados,int tamanho,int quant,int cont){
    //Processo de inserção. Usuário digita dados a serem inseridos
    series novosDados[quant];
    for (int i = 0; i < quant; i++){
        cin.ignore();
        if (cont >= quant){
            novosDados[i].identificador = ((tamanho-cont)+i)+1;
        } else {
            novosDados[i].identificador = ((tamanho-quant)+i)+1;
        }
        cout << endl << "Nome: ";
        cin.getline(novosDados[i].nome,45); 
        cout << "Ano de Lançamento: ";
        cin >> novosDados[i].data;
        cin.ignore();
        cout << "Personagem Principal (Ator): ";
        cin.getline(novosDados[i].personagem,55);
        cout << "Quantidade de Visualizações na Estreia: ";
        cin >> novosDados[i].visualizacao;
        cout << "Aprovação do Público: ";
        cin >> novosDados[i].aprovacao;
        cin.ignore();
        cout << "Gênero: ";
        cin.getline(novosDados[i].genero,35);
        cout << endl << "Dado inserido com sucesso!";
        if (quant > 1 and i != (quant-1)){
            cout << endl << "Dê Enter para inserir mais dados!";
        }
    }

    for (int j = 0; j < quant; j++){
        if (cont >= quant){
            dados[(tamanho-cont)+j] = novosDados[j];
        } else {
            dados[(tamanho-quant)+j] = novosDados[j];
        }
    }
}

//Função para imprimir dados do arquivo
void impressao(series dados[],int tam){
    //Escolha do procedimento de Impressão
    int proc;
    perguntas(4);
    cin >> proc;
    //Impressão de todos os dados do arquivo
    if (proc == 1){
        system("clear");
        cout << endl << "---------------------------------- Resultado: ----------------------------------" << endl << endl;
        for (int i=0;i<tam and dados[i].identificador != -1;i++){
            cout << dados[i].identificador << ": "
                << dados[i].nome << ";" 
                << dados[i].data << ";"
                << dados[i].personagem << ";" 
                << dados[i].visualizacao << ";"
                << dados[i].aprovacao << ";"
                << dados[i].genero << endl;
        }
        cout << endl << "--------------------------------------------------------------------------------" << endl << endl;
    }
    //Impressão de um trecho específico do arquivo
    else if (proc == 2){
        int posInicial, posFinal;
        cout << "Posição Inicial: ";
        cin >> posInicial;
        cout << "Posição Final: ";
        cin >> posFinal;
        system("clear");
        cout << endl << "---------------------------------- Resultado: ----------------------------------" << endl << endl;
        if (posFinal < posInicial or posInicial > tam or posFinal > tam or dados[posFinal-1].identificador == -1){
            cout << setw(31) << setfill(' ') << "" << "Posições Invalidas" << endl;
        } else {
            for (int i=(posInicial-1);i<posFinal;i++){
                cout << dados[i].identificador << ": "
                    << dados[i].nome << ";" 
                    << dados[i].data << ";"
                    << dados[i].personagem << ";" 
                    << dados[i].visualizacao << ";"
                    << dados[i].aprovacao << ";"
                    << dados[i].genero << endl;
            }
        }
        cout << endl << "--------------------------------------------------------------------------------" << endl << endl;
    } else {
        system("clear");
        cout << endl << "----------------------------------" << endl << endl;
        cout << setw(5) << setfill(' ') << "" << "Procedimento Inválido!" << endl << endl;
    }
}

//Função para salvar os dados no arquivo .csv
void salvar_csv(series dados[],int tam,char info[]){
    int op1;
    perguntas(6);
    cin >> op1;
    int cont = 0;
    bool parar = false;
    for (int i = (tam - 1); i >= 0 and !parar; i--){
        if (dados[i].identificador == -1){
            cont++;
        } else {
            parar = true;
        }
    }
    if (op1 == 1){
        ofstream saida("serie.csv");
        saida << info << endl << tam-cont << endl;
        for (int i = 0;i < tam and dados[i].identificador != -1;i++){
            saida << dados[i].nome << ";" 
                << dados[i].data << ";"
                << dados[i].personagem << ";" 
                << dados[i].visualizacao << ";"
                << dados[i].aprovacao << ";"
                << dados[i].genero << endl;
        }
        system("clear");
        cout << endl << "----------------------------------" << endl << endl;
        cout << "Dados salvos no arquivo serie.csv com sucesso!" << endl;
        cout << endl << "----------------------------------" << endl << endl;
    } else if (op1 == 2){
        string nomearq;
        cout << "Digite o nome do novo arquivo ('nome.csv'): ";
        cin >> nomearq;
        ofstream saida(nomearq);
        saida << info << endl << tam-cont<< endl;
        for (int i = 0;i < tam and dados[i].identificador != -1;i++){
            saida << dados[i].nome << ";" 
                << dados[i].data << ";"
                << dados[i].personagem << ";" 
                << dados[i].visualizacao << ";"
                << dados[i].aprovacao << ";"
                << dados[i].genero << endl;
        }
        system("clear");
        cout << endl << "----------------------------------" << endl << endl;
        cout << "Dados salvos no arquivo " << nomearq << " com sucesso!" << endl;
        cout << endl << "----------------------------------" << endl << endl;
    } else {
        system("clear");
        cout << endl << "----------------------------------" << endl << endl;
        cout << setw(5) << setfill(' ') << "" << "Procedimento Inválido!" << endl << endl;
        cout << endl << "----------------------------------" << endl << endl;
    }
}

//Função que retorna quantos dados foram removidos (usados especialmente na lixeira e restauração)
int quantRemoFuncao(series dados[],int tam){
    int quantRemo = 0;
    for (int i = tam - 1; i >= 0 and dados[i].identificador == -1; i--){
        quantRemo++;
    }
    return quantRemo;
}

//Função que mostra ao usuário os dados que foram removidos
void lixeira(series dados[],int tam,int quantRemo){    
    int i = (tam - quantRemo);

    system("clear");
    cout << endl << "---------------------------------- Resultado: ----------------------------------" << endl << endl;
    if (quantRemo != 0){
        int id = 1;
        while (i < tam){
            cout << id << ": "
                << dados[i].nome << ";" 
                << dados[i].data << ";"
                << dados[i].personagem << ";" 
                << dados[i].visualizacao << ";"
                << dados[i].aprovacao << ";"
                << dados[i].genero << endl;
            i++;
            id++;
        }
    } else {
        cout << setw(37) << setfill(' ') << "" << "Vazio!" << endl;
    }
    cout << endl << "--------------------------------------------------------------------------------" << endl << endl;
}

//Função que restaura os dados que o usuário quiser
void restauracao(series* &dados,int tam,int comando){
    if (comando == 1){
        //Conta quantos dados estão removidos
        int quantRemo = quantRemoFuncao(dados,tam);

        char rest[45];
        cout << endl;
        cout << "Digite o nome da série que deseja restaurar: ";
        cin.ignore();
        cin.getline(rest,45);

        bool achou = false;
        for (int j = (tam - quantRemo) - 1; j < tam and !achou; j++){
            if (strcmp(dados[j].nome,rest) == 0){
                for (int h = tam - 1; h >= 0 and !achou; h--){
                    if (dados[h].identificador != -1){
                        if ((h+1) == j){
                            achou = true;
                            dados[j].identificador = ((tam-quantRemo)+1);
                        } else{
                            dados[j].identificador = ((tam-quantRemo)+1);
                            swap(dados[j],dados[h+1]);
                            achou = true;
                        }
                        
                    }                       
                }
                system("clear");
                cout << endl << "----------------------------------" << endl << endl;
                cout << setw(4) << setfill(' ') << "" << "Dado restaurado com sucesso" << endl;
                cout << endl << "----------------------------------" << endl << endl;
            }
        }
        if (!achou){
            system("clear");
            cout << endl << "--------------------------------------------------------------------------------" << endl << endl;
            cout << setw(26) << setfill(' ') << "" << "Dado inexistente na lixeira!" << endl;
            cout << endl << "--------------------------------------------------------------------------------" << endl << endl;
        }
    } else if (comando == 2){
        int quantRest;
        cout << endl;
        cout << "Quantos dados deseja restaurar?: ";
        cin >> quantRest;
        int quantRemo = quantRemoFuncao(dados,tam);
        if (quantRest <= quantRemo){
            for (int i = 0; i < quantRest; i++){
                char rest[45];
                cout << endl;
                cout << "Digite o nome da série que deseja restaurar: ";
                if (i == 0)
                    cin.ignore();
                cin.getline(rest,45);
                bool achou = false;
                for (int j = (tam - quantRemo) - 1; j < tam and !achou; j++){
                    if (strcmp(dados[j].nome,rest) == 0){
                        for (int h = tam - 1; h >= 0 and !achou; h--){
                            if (dados[h].identificador != -1){
                                if ((h+1) == j){
                                    achou = true;
                                    dados[j].identificador = ((tam-quantRemo)+1);
                                } else{
                                    dados[j].identificador = ((tam-quantRemo)+1);
                                    swap(dados[j],dados[h+1]);
                                    achou = true;
                                }
                                
                            }                       
                        }
                        system("clear");
                        cout << endl << "----------------------------------" << endl << endl;
                        cout << setw(4) << setfill(' ') << "" << "Dado restaurado com sucesso" << endl;
                        cout << endl << "----------------------------------" << endl;
                    }
                }
                if (!achou){
                    system("clear");
                    cout << endl << "--------------------------------------------------------------------------------" << endl << endl;
                    cout << setw(26) << setfill(' ') << "" << "Dado inexistente na lixeira!" << endl;
                    cout << endl << "--------------------------------------------------------------------------------" << endl << endl;
                    i--;
                }
                quantRemo = quantRemoFuncao(dados,tam);
            }
        } else {
            cout << endl << "----------------------------------" << endl << endl;
            cout << setw(7) << setfill(' ') << "" << "Quantidade Inválida" << endl << endl;
            cout << "----------------------------------" << endl;
        }
    } else if (comando == 3){
        int quantRemo = quantRemoFuncao(dados,tam);
        for (int i = (tam - quantRemo); i < tam and dados[i].identificador == -1; i++){
            dados[i].identificador = i+1;
        }
        system("clear");
        cout << endl << "----------------------------------" << endl << endl;
        cout << setw(2) << setfill(' ') << "" << "Dados restaurados com sucesso!" << endl << endl;
        cout << "----------------------------------" << endl;
    }
}

int main(){
    //Entrada dos dados do arquivo .csv
    ifstream entrada("serie.csv");

	//Leitura dos dados
	if (entrada.is_open()){
        char info[150];
        int tamanho;
        entrada.getline(info,150);
        entrada >> tamanho;
        entrada.ignore();
        series *dados_registro = new series[tamanho];
        gravacao_em_registro(dados_registro,entrada,tamanho);
        arquivo_binario_escrita(dados_registro,tamanho);
        delete[] dados_registro;
        
        int resultado_procedimento;
        bool parar = false;
        do {
            resultado_procedimento = procedimento();
            //Abertura do arquivo binário
            fstream arquivo_binario("series_binario.dat", ios::binary | ios::in | ios::out);
            if (arquivo_binario.is_open()){
                //Contagem de bytes(caracteres)
                arquivo_binario.seekg(0, arquivo_binario.end);
                streampos tamanho_binario = arquivo_binario.tellg();
                arquivo_binario.seekg(0, arquivo_binario.beg);

                //Variável com a quantidade de registros
                int numRegistros = tamanho_binario / sizeof(series);

                //Alocação dos dados do arquivo binário em um vetor de registros
                series *dados = new series[numRegistros];
                arquivo_binario.read((char*)(dados), tamanho_binario);

                //Escolha do procedimento
                if (resultado_procedimento == 1){
                    //Busca
                    char campo;
                    perguntas(1);
                    cin >> campo;
                    cout << endl << "----------------------------------" << endl << endl;

                    if (toupper(campo) == 'N'){
                        char buscador[45];    
                        cout << "Nome: ";
                        cin.ignore();
                        cin.getline(buscador,45);
                        shell_sort(dados,numRegistros,0);
                        busca_binaria_nome(dados,buscador,0,(numRegistros-1));
                    } else if (toupper(campo) == 'G') {
                        busca(dados,numRegistros);
                    } else {
                        system("clear");
                        cout << endl << "----------------------------------" << endl << endl;
                        cout << setw(9) << setfill(' ') << "" << "Campo inválido!" << endl << endl;
                    }
                } else if (resultado_procedimento == 2){
                    //Inserção
                    int quant;
                    cout << "Quantos dados deseja inserir: ";
                    cin >> quant;
                    if (quant > 0){
                        int cont = 0;
                        bool parar = false;
                        for (int i = (numRegistros - 1); i >= 0 and !parar; i--){
                            if (dados[i].identificador == -1){
                                cont++;
                            } else {
                                parar = true;
                            }
                        }
                        
                        if (cont >= quant){
                            insercao(dados,numRegistros,quant,cont);
                        } else {
                            int auxQuant = quant - cont;
                            if (cont == 0){
                                cont++;
                            }
                            redimensionar(dados,numRegistros,auxQuant);
                            insercao(dados,numRegistros,quant,cont);
                        }

                        arquivo_binario.seekg(0, ios::beg);
                        arquivo_binario.write((char*)(dados), sizeof(series) * numRegistros);
                        int op;
                        perguntas(2);
                        cin >> op;
                        cout << endl << "----------------------------------" << endl << endl;
                        if (op == 1){
                            salvar_csv(dados,numRegistros,info);
                        } else if (op == 2) {
                            impressao(dados,numRegistros);
                        } else {
                            cout << setw(5) << setfill(' ') << "" << "Procedimento Inválido!" << endl << endl;
                        }
                    } else {
                        cout << endl << endl << "Quantidade Inválida" << endl << endl;
                    }
                } else if (resultado_procedimento == 3){
                    //Impressão
                    impressao(dados,numRegistros);
                } else if (resultado_procedimento == 4){
                    //Ordenação
                    int campo;
                    perguntas(3);
                    cin >> campo;
                    cout << endl << "----------------------------------" << endl;
                    if (campo == 1 or campo == 2){
                        shell_sort(dados,numRegistros,campo);
                        arquivo_binario.seekg(0, ios::beg);
                        arquivo_binario.write((char*)(dados), sizeof(series) * numRegistros);
                        if (campo != -1){
                            int op2;
                            perguntas(2);
                            cin >> op2;
                            cout << endl << "----------------------------------" << endl << endl;
                            if (op2 == 1){
                                salvar_csv(dados,numRegistros,info);
                            } else if (op2 == 2){
                                impressao(dados,numRegistros);
                            } else {
                                cout << setw(5) << setfill(' ') << "" << "Procedimento Inválido!" << endl << endl;
                            }
                        }
                    } else {
                        cout << setw(5) << setfill(' ') << "" << "Procedimento Inválido!" << endl << endl;
                    }
                } else if (resultado_procedimento == 5){
                    remocao(dados,numRegistros);

                    arquivo_binario.seekg(0, ios::beg);
                    arquivo_binario.write((char*)(dados), sizeof(series) * numRegistros);
                } else if (resultado_procedimento == 6){
                    int quantRemo = quantRemoFuncao(dados,numRegistros);
                    
                    lixeira(dados,numRegistros,quantRemo);
                    if (quantRemo != 0){
                        perguntas(9);
                        int comando;
                        cin >> comando;
                        restauracao(dados,numRegistros,comando);
                        
                        arquivo_binario.seekg(0, ios::beg);
                        arquivo_binario.write((char*)(dados), sizeof(series) * numRegistros);
                    }
                } else if (resultado_procedimento == 0){
                    char sn;
                    perguntas(7);
                    cin >> sn;
                    cout << endl << "----------------------------------" << endl << endl;
                    if (toupper(sn) == 'S'){
                        salvar_csv(dados,numRegistros,info);
                        parar = true;
                    } else {
                        char sn2;
                        perguntas(8);
                        cin >> sn2;
                        cout << endl << "----------------------------------" << endl << endl;
                        if (toupper(sn2) == 'S'){
                            parar = true;
                        } else {
                            salvar_csv(dados,numRegistros,info);
                            parar = true;
                        } 

                    }
                } else {
                    cout << setw(5) << setfill(' ') << "" << "Procedimento Inválido!" << endl << endl;
                }
                arquivo_binario.close();
                delete[] dados;
            } else {
                cout << endl << "Arquivo Binário com Problemas!" << endl;
            }
        } while (resultado_procedimento != 0 and !parar);
        cout << setw(10) << setfill(' ') << "" << "Até mais! :)" << endl;
        cout << endl << "----------------------------------" << endl << endl;
    } else {
        cout << "Arquivo CSV com Problemas!!!" << endl << endl;
    }
    return 0;
}