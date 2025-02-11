#include <iostream>
#include <stdexcept>
#include "data.h"

using namespace std;

int main(){
    try {
        // Instancia um objeto da classe SequenceSet e carreda o .csv
        SequenceSet ss;
        ss.carregarArquivo("Subnational-period-life-tables-2017-2019-CSV.csv", false);
        exibirIntro();
        char opcao;
        do { // Menu de opções
            menu();
            cin >> opcao;
            switch (opcao) {
            case '1': //Inserção via entrada padrão
                system("clear");
                layoutInsercaoPadrao();
                ss.inserirViaEntradaPadrao();
                break;
            case '2': { // Inserção via arquivo texto
                string nomeArqTxt;
                system("clear");
                layoutInsercaoPorTxt();
                cin >> nomeArqTxt;
                ss.carregarArquivo(nomeArqTxt,true);
                break;
            }
            case '3': { // Remoção de dado específico
                int tipoRemocao;
                system("clear");
                layoutRemocao();
                cin >> tipoRemocao;

                if (tipoRemocao == 1){
                    string chaveMedida, chaveIdade, chaveEtnia;
                    printf("%-36s", "\n› Informe a medida para remover: ");
                    cin >> chaveMedida;

                    printf("%-35s", "› Informe a idade para remover: ");
                    cin.ignore();
                    getline(cin, chaveIdade);

                    printf("%-35s", "› Informe a etnia para remover: ");
                    cin >> chaveEtnia;
                                        
                    ss.removerRegistro(chaveMedida, chaveIdade, chaveEtnia);
                } else if (tipoRemocao == 2){
                    string chaveMedida, chaveQuantil, chaveArea, chaveSexo, chaveIdade, chaveRegiao, chaveEtnia;

                    printf("%-37s", "\n› Informe a medida para remover: ");
                    cin >> chaveMedida;

                    printf("%-36s", "› Informe o quantil para remover: ");
                    cin >> chaveQuantil;
                    
                    printf("%-37s", "› Informe a área para remover: ");
                    cin.ignore();
                    getline(cin, chaveArea);

                    printf("%-36s", "› Informe o sexo para remover: ");
                    cin >> chaveSexo;

                    printf("%-36s", "› Informe a idade para remover: ");
                    cin.ignore();
                    getline(cin, chaveIdade);

                    printf("%-37s", "› Informe a região para remover: ");
                    getline(cin, chaveRegiao);

                    printf("%-36s", "› Informe a etnia para remover: ");
                    cin >> chaveEtnia;

                    ss.removerRegistro(chaveMedida, chaveQuantil, chaveArea, chaveSexo, chaveIdade, chaveRegiao, chaveEtnia);
                } else {
                    cout << "\n ### Opção inválida! ###\n";
                }
                
                break;
            }
            case '4': { // Busca de dado
                int tipoBusca;
                system("clear");
                layoutBusca();
                cin >> tipoBusca;
                
                if (tipoBusca == 1){ // Busca apenas por medida, idade e etnia
                    string chaveMedida, chaveIdade, chaveEtnia;
                    printf("%-36s", "\n› Informe a medida para buscar: ");
                    cin >> chaveMedida;

                    printf("%-35s", "› Informe a idade para buscar: ");
                    cin.ignore();
                    getline(cin, chaveIdade);

                    printf("%-35s", "› Informe a etnia para buscar: ");
                    cin >> chaveEtnia;
                    
                    ss.buscarRegistro(chaveMedida, chaveIdade, chaveEtnia);
                    break;
                } else if (tipoBusca == 2){ // Busca por todos os campos
                    string chaveMedida, chaveQuantil, chaveArea, chaveSexo, chaveIdade, chaveRegiao, chaveEtnia;
                    printf("%-36s", "\n› Informe a medida para buscar: ");
                    cin >> chaveMedida;

                    printf("%-35s", "› Informe o quantil para buscar: ");
                    cin >> chaveQuantil;
                    
                    printf("%-36s", "› Informe a área para buscar: ");
                    cin.ignore();
                    getline(cin, chaveArea);

                    printf("%-35s", "› Informe o sexo para buscar: ");
                    cin >> chaveSexo;

                    printf("%-35s", "› Informe a idade para buscar: ");
                    cin.ignore();
                    getline(cin, chaveIdade);

                    printf("%-36s", "› Informe a região para buscar: ");
                    getline(cin, chaveRegiao);

                    printf("%-35s", "› Informe a etnia para buscar: ");
                    cin >> chaveEtnia;

                    ss.buscarRegistro(chaveMedida, chaveQuantil, chaveArea, chaveSexo, chaveIdade, chaveRegiao, chaveEtnia);
                    break;
                } else { //Opção inválida
                    cout << "\n ### Opção inválida! ###\n";
                    break;
                }
            }
            case '5': { // Salvar os dados atuais em um arquivo
                int opcaoArq;
                system("clear");
                layoutSalvarArquivo(0);
                cin >> opcaoArq;
                if (opcaoArq == 1){
                    string nomeArqTxt;
                    system("clear");
                    layoutSalvarArquivo(opcaoArq);
                    cin.ignore();
                    getline(cin, nomeArqTxt);
                    ss.salvarEmTxt(nomeArqTxt + ".txt");
                    break;
                } else if (opcaoArq == 2){
                    string nomeArqCsv;
                    system("clear");
                    layoutSalvarArquivo(opcaoArq);
                    cin.ignore();
                    getline(cin, nomeArqCsv);
                    ss.salvarEmCsv(nomeArqCsv + ".csv");
                    break;
                } else {
                    cout << "\n ### Opção inválida! ###\n";
                    break;
                }
                
            }
            case '0': // Sair
                char yesNo;
                cout << "\n» Desejar realmente sair? *Todas as alterações não salvas serão descartadas* (s/n): ";
                cin >> yesNo;
                if (yesNo == 's') {
                    ss.salvarEmTxt("registros.txt");
                    system("clear");
                    layoutSair();
                    break;
                } else if (yesNo == 'n') {
                    opcao = '$';
                    system("clear");
                } else {
                    system("clear");
                    cout << "\n ### Opção inválida! ###\n";
                    opcao = '$';
                    break;
                }
            case '$':
                exibirIntro();
                break;
            default:
                cout << "\n ### Opção inválida! ###\n";
            }
        } while (opcao != '0');
    } catch (const exception &e) { // Tratamento de exceção
        cerr << "Erro: " << e.what() << endl;
    }
    
    return 0;
}