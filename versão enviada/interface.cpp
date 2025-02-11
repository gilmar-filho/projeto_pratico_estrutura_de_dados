#include <iostream>
#include <cstdlib>  // Para usar system e popen
#include <cmath> // Para usar floor
#include "data.h" // Incluindo o arquivo de cabeçalho da classe SequenceSet
using namespace std;

// Função para exibir a tabela{
void exibirTabela(Dado dado, bool encontrou) {

    if (!encontrou){
        cout << "\n» Exibindo registro(s) encontrado(s)\n";
        cout << "┌────────────────────┬─────────────────────┬─────────────────────┬─────────────────────┬─────────────────────┐\n";
        printf("%-9s %-12s %-9s %-13s %-10s %-12s %-10s %-12s %-10s %-12s %s\n",
            "│", "Medida", "│","Quantil", "│", "Idade" , "│", "Etnia", "│", "Valor ", "│");
    }
    cout << "├────────────────────┼─────────────────────┼─────────────────────┼─────────────────────┼─────────────────────┤\n";

    // Impressão condicional do valor (inteiro ou decimal)
    printf("│ %-18s │ %-19s │ %-19s │ %-19s │", dado.medida, dado.quantil, dado.idade, dado.etnia);

    if (floor(dado.valor) == dado.valor) {
        // Valor é inteiro
        printf(" %-19d │\n", static_cast<int>(dado.valor));
    } else {
        // Valor é decimal
        printf(" %-19f │\n", dado.valor);
    }
}

// Função para exibir um registro
void exibirRegistro(Dado dado){

    cout << "┌────────────┬──────────────────────────────────────────────────────────────┐\n";
    printf("│ %-10s │ %-60s │\n", "Medida", dado.medida);
    cout << "├────────────┼──────────────────────────────────────────────────────────────┤\n";
    printf("│ %-10s │ %-60s │\n", "Quantil", dado.quantil);
    cout << "├────────────┼──────────────────────────────────────────────────────────────┤\n";
    printf("│ %-11s │ %-60s │\n", "Área", dado.area);
    cout << "├────────────┼──────────────────────────────────────────────────────────────┤\n";
    printf("│ %-10s │ %-60s │\n", "Sexo", dado.sex);
    cout << "├────────────┼──────────────────────────────────────────────────────────────┤\n";
    printf("│ %-10s │ %-60s │\n", "Idade", dado.idade);
    cout << "├────────────┼──────────────────────────────────────────────────────────────┤\n";
    printf("│ %-11s │ %-60s │\n", "Região", dado.regiao);
    cout << "├────────────┼──────────────────────────────────────────────────────────────┤\n";
    printf("│ %-10s │ %-60s │\n", "Etnia", dado.etnia);
    cout << "├────────────┼──────────────────────────────────────────────────────────────┤\n";

    if (floor(dado.valor) == dado.valor) {
        // Valor é inteiro
        printf("│ %-10s │ %-60d │\n", "Valor", static_cast<int>(dado.valor));
    } else {
        // Valor é decimal
        printf("│ %-10s │ %-60f │\n", "Valor", dado.valor);
    }
    
    cout << "└────────────┴──────────────────────────────────────────────────────────────┘\n";

}

// Função para obter a largura do terminal
int obterLarguraTerminal() {
    FILE* pipe = popen("tput cols", "r"); // Executa o comando `tput cols`
    if (!pipe) return 80; // Valor padrão em caso de erro
    char buffer[128];
    string resultado = "";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        resultado += buffer;
    }
    pclose(pipe);
    return stoi(resultado);
}

// Função para centralizar texto
void imprimirCentralizado(const string& texto) {
    int larguraTerminal = obterLarguraTerminal();
    int espacos = (larguraTerminal - texto.size()) / 2;
    if (espacos > 0) {
        cout << string(espacos, ' ') << texto << endl;
    } else {
        cout << texto << endl;
    }
}


void exibirIntro() {

    cout << endl << endl;
    imprimirCentralizado("#==========================================================================#\n");
    imprimirCentralizado("## SISTEMA DE REGISTRO DE DADOS ##");
    imprimirCentralizado("## (Sequence Set) ##\n");
    imprimirCentralizado("#==========================================================================#\n");
    imprimirCentralizado("## Tabelas de Vida Subnacionais: Taxas de Mortalidade e Expectativa de ##");
    imprimirCentralizado("## Vida por Região, Sexo, Idade e Grupo Étnico (2017-2019) ##\n");
    imprimirCentralizado("#==========================================================================#\n");

}

// Função para exibir o menu
void menu(){
    cout << "\n╔══════════════════════════════════════════════════╗\n"
         <<   "║  ┌────────────────────────────────────────────┐  ║\n"
         <<   "║  │ »»»»»»»»»»»»»»»»»» MENU «««««««««««««««««« │  ║\n"
         <<   "║  └────────────────────────────────────────────┘  ║\n"
         <<   "╠═══╦══════════════════════════════════════════════╣\n"
         <<   "║ 1 ║ Inserir registro via entrada padrão          ║\n"
         <<   "╠═══╬══════════════════════════════════════════════╣\n"
         <<   "║ 2 ║ Inserir registros via arquivo texto          ║\n"
         <<   "╠═══╬══════════════════════════════════════════════╣\n"
         <<   "║ 3 ║ Remover registro                             ║\n"
         <<   "╠═══╬══════════════════════════════════════════════╣\n"
         <<   "║ 4 ║ Buscar registro                              ║\n"
         <<   "╠═══╬══════════════════════════════════════════════╣\n"
         <<   "║ 5 ║ Salvar os dados atuais em um arquivo         ║\n"
         <<   "╠═══╬══════════════════════════════════════════════╣\n"
         <<   "║ 0 ║ Sair                                         ║\n"
         <<   "╚═══╩══════════════════════════════════════════════╝\n"
         <<   "\n» Informe a operação que deseja realizar: ";
}

// Função para exibir o layout de inserção padrão
void layoutInsercaoPadrao(){
    cout << "\n╔══════════════════════════════════════════════════╗\n"
         <<   "║  ┌────────────────────────────────────────────┐  ║\n"
         <<   "║  │ »»»»»»»»»»»» INSERÇÃO PADRÃO ««««««««««««« │  ║\n"
         <<   "║  └────────────────────────────────────────────┘  ║\n"
         <<   "╚══════════════════════════════════════════════════╝\n"
         <<   "\n» Informe os dados do registro\n";
}

// Função para exibir o layout de inserção por arquivo texto
void layoutInsercaoPorTxt(){ 

    cout << "\n╔══════════════════════════════════════════════════╗\n"
         <<   "║  ┌────────────────────────────────────────────┐  ║\n"
         <<   "║  │ »»»»»»» INSERÇÃO POR ARQUIVO TEXTO ««««««« │  ║\n"
         <<   "║  └────────────────────────────────────────────┘  ║\n"
         <<   "╚══════════════════════════════════════════════════╝\n"
         <<   "\n» Informe o nome do arquivo texto: ";
}

// Função para exibir o layout de remoção
void layoutRemocao(){ 

    cout << "\n╔══════════════════════════════════════════════════╗\n"
         <<   "║  ┌────────────────────────────────────────────┐  ║\n"
         <<   "║  │ »»»»»»»»»» REMOÇÃO DE REGISTRO ««««««««««« │  ║\n"
         <<   "║  └────────────────────────────────────────────┘  ║\n"
         <<   "╠═══╦══════════════════════════════════════════════╣\n"
         <<   "║ 1 ║ Remoção por medida, idade e etnia (vários).  ║\n"
         <<   "╠═══╬══════════════════════════════════════════════╣\n"
         <<   "║ 2 ║ Remoção por todos os campos (específico).    ║\n"
         <<   "╚═══╩══════════════════════════════════════════════╝\n"
         <<   "\n» Informe o tipo de remoção: ";

}

// Função para exibir o layout de busca
void layoutBusca(){ 
    cout << "\n╔══════════════════════════════════════════════════╗\n"
         <<   "║  ┌────────────────────────────────────────────┐  ║\n"
         <<   "║  │ »»»»»»»»»»» BUSCA DE REGISTRO «««««««««««« │  ║\n"
         <<   "║  └────────────────────────────────────────────┘  ║\n"
         <<   "╠═══╦══════════════════════════════════════════════╣\n"
         <<   "║ 1 ║ Busca por medida, idade e etnia (vários).    ║\n"
         <<   "╠═══╬══════════════════════════════════════════════╣\n"
         <<   "║ 2 ║ Busca por todos os campos (específico).      ║\n"
         <<   "╚═══╩══════════════════════════════════════════════╝\n"
         <<   "\n» Informe o tipo de busca: ";
}

// Função para exibir o layout de salvar em txt
void layoutSalvarArquivo(int opcao){

    if (opcao == 0){
    cout << "\n╔══════════════════════════════════════════════════╗\n"
         <<   "║  ┌────────────────────────────────────────────┐  ║\n"
         <<   "║  │ »»»»»»»»»»» SALVAR EM ARQUIVO «««««««««««« │  ║\n"
         <<   "║  └────────────────────────────────────────────┘  ║\n"
         <<   "╠═══╦══════════════════════════════════════════════╣\n"
         <<   "║ 1 ║ Salvar em arquivo '.txt'.                    ║\n"
         <<   "╠═══╬══════════════════════════════════════════════╣\n"
         <<   "║ 2 ║ Salvar em arquivo '.csv'                     ║\n"
         <<   "╚═══╩══════════════════════════════════════════════╝\n"
         <<   "\n» Informe o tipo de arquivo: ";
    }
    
    if (opcao == 1){
        cout << "\n╔══════════════════════════════════════════════════╗\n"
             <<   "║  ┌────────────────────────────────────────────┐  ║\n"
             <<   "║  │ »»»»»»»»» SALVAR EM ARQUIVO .TXT ««««««««« │  ║\n"
             <<   "║  └────────────────────────────────────────────┘  ║\n"
             <<   "╚══════════════════════════════════════════════════╝\n"
             <<   "\n» Informe como deseja nomear o arquivo: ";
    }
    if (opcao == 2){
         cout << "\n╔══════════════════════════════════════════════════╗\n"
             <<   "║  ┌────────────────────────────────────────────┐  ║\n"
             <<   "║  │ »»»»»»»»» SALVAR EM ARQUIVO .CSV ««««««««« │  ║\n"
             <<   "║  └────────────────────────────────────────────┘  ║\n"
             <<   "╚══════════════════════════════════════════════════╝\n"
             <<   "\n» Informe como deseja nomear o arquivo: ";
    }
}

// Função para exibir o layout de sair
void layoutSair(){ 
    cout << "\n╔══════════════════════════════════════════════════╗\n"
         <<   "║  ┌────────────────────────────────────────────┐  ║\n"
         <<   "║  │ »»»»»»»»»»»»»» ATÉ MAIS !!! «««««««««««««« │  ║\n"
         <<   "║  └────────────────────────────────────────────┘  ║\n"
         <<   "╚══════════════════════════════════════════════════╝\n"
         <<   "\n» Saindo...";
}