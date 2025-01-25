#include <iostream>
#include <stdexcept>
#include "data.h"

using namespace std;

int main(){
    try {
        // Instancia um objeto da classe SequenceSet e carreda o .csv
        SequenceSet ss;
        ss.carregarArquivo("Subnational-period-life-tables-2017-2019-CSV.csv", false);
        int opcao;
        do { // Menu de opções
            menu();
            cin >> opcao;

            switch (opcao) {
            case 1: //Inserção via entrada padrão
                ss.inserirViaEntradaPadrao();
                break;
            case 2: { // Inserção via arquivo texto
                string nomeArqTxt;
                cout << "Informe o nome do arquivo texto: ";
                cin >> nomeArqTxt;
                ss.carregarArquivo(nomeArqTxt,true);
                break;
            }
            case 3: { // Remoção de dado específico
                int tipoRemocao;
                cout << "   1. Remoção por medida, idade e etnia\n";
                cout << "   2. Remoção por todos os campos\n";
                cout << "   Escolha uma opção: ";
                cin >> tipoRemocao;

                if (tipoRemocao == 1){
                    string chaveMedida, chaveIdade, chaveEtnia;
                    cout << "Informe a medida: ";
                    cin >> chaveMedida;

                    cout << "Informe a idade: ";
                    cin.ignore();
                    getline(cin, chaveIdade);

                    cout << "Informe a etnia: ";
                    cin >> chaveEtnia;
                    
                    ss.removerRegistro(chaveMedida, chaveIdade, chaveEtnia);
                } else if (tipoRemocao == 2){
                    string chaveMedida, chaveQuantil, chaveArea, chaveSexo, chaveIdade, chaveRegiao, chaveEtnia;
                    double chaveValor;
                    cout << "Informe a medida para remover: ";
                    cin >> chaveMedida;

                    cout << "Informe o quantil para remover: ";
                    cin >> chaveQuantil;
                    
                    cout << "Informe a área para remover: ";
                    cin.ignore();
                    getline(cin, chaveArea);

                    cout << "Informe o sexo para remover: ";
                    cin >> chaveSexo;

                    cout << "Informe a idade para remover: ";
                    cin.ignore();
                    getline(cin, chaveIdade);

                    cout << "Informe a região para remover: ";
                    getline(cin, chaveRegiao);

                    cout << "Informe a etnia para remover: ";
                    cin >> chaveEtnia;

                    cout << "Informe o valor para remover: ";
                    cin >> chaveValor;

                    ss.removerRegistro(chaveMedida, chaveQuantil, chaveArea, chaveSexo, chaveIdade, chaveRegiao, chaveEtnia, chaveValor);
                } else {
                    cout << "Opção inválida!\n";
                }
                
                break;
            }
            case 4: { // Busca de dado
                int tipoBusca;
                cout << "   1. Busca por medida, idade e etnia\n";
                cout << "   2. Busca por todos os campos\n";
                cout << "   Escolha uma opção: ";
                cin >> tipoBusca;
                
                if (tipoBusca == 1){ // Busca apenas por medida, idade e etnia
                    string chaveMedida, chaveIdade, chaveEtnia;
                    cout << "Informe a medida para buscar: ";
                    cin >> chaveMedida;

                    cout << "Informe a idade para buscar: ";
                    cin.ignore();
                    getline(cin, chaveIdade);

                    cout << "Informe a etnia para buscar: ";
                    cin >> chaveEtnia;
                    
                    ss.buscarRegistro(chaveMedida, chaveIdade, chaveEtnia);
                    break;
                } else if (tipoBusca == 2){ // Busca por todos os campos
                    string chaveMedida, chaveQuantil, chaveArea, chaveSexo, chaveIdade, chaveRegiao, chaveEtnia;
                    double chaveValor;
                    cout << "Informe a medida para buscar: ";
                    cin >> chaveMedida;

                    cout << "Informe o quantil para buscar: ";
                    cin >> chaveQuantil;
                    
                    cout << "Informe a área para buscar: ";
                    cin.ignore();
                    getline(cin, chaveArea);

                    cout << "Informe o sexo para buscar: ";
                    cin >> chaveSexo;

                    cout << "Informe a idade para buscar: ";
                    cin.ignore();
                    getline(cin, chaveIdade);

                    cout << "Informe a região para buscar: ";
                    getline(cin, chaveRegiao);

                    cout << "Informe a etnia para buscar: ";
                    cin >> chaveEtnia;

                    cout << "Informe o valor para buscar: ";
                    cin >> chaveValor;

                    ss.buscarRegistro(chaveMedida, chaveQuantil, chaveArea, chaveSexo, chaveIdade, chaveRegiao, chaveEtnia, chaveValor);
                    break;
                } else { //Opção inválida
                    cout << "Opção inválida!\n";
                    break;
                }
            }
            case 5: { // Salvar os dados atuais em um arquivo .txt
                string nomeArqTxt;
                cout << "Informe o nome do arquivo texto: ";
                cin >> nomeArqTxt;
                ss.salvarEmTxt(nomeArqTxt);
                break;
            }
            case 0: // Sair
                ss.salvarEmTxt("registros.txt");
                cout << "Saindo...\n";
                break;
            default:
                cout << "Opção inválida!\n";
            }
        } while (opcao != 0);
    } catch (const exception &e) { // Tratamento de exceção
        cerr << "Erro: " << e.what() << endl;
    }
    /*ss.removerRegistro("px", 0.57022);
    ss.removerRegistro("px", 0.99402);

    ss.carregarArquivo("registros.txt", true);
    
    ss.adicionarRegistro({"px", "250%", "Teste", "Teste", "Teste", "Teste", "Teste", 0.123});
    ss.adicionarRegistro({"tx", "250%", "Teste", "Teste", "Teste", "Teste", "Teste", 0.456});
    ss.adicionarRegistro({"yx", "250%", "Teste", "Teste", "Teste", "Teste", "Teste", 0.789});

    //ss.buscarRegistro("px", "80-84 years", "Total");
    //ss.buscarRegistro("px", "Teste", "Teste");
    //ss.buscarRegistro("px", "2.50%", "07 Taranaki region", "Female", "80-84 years", "Regional council", "Total", 0.73948);
    ss.buscarRegistro("px", "250%", "Teste", "Teste", "Teste", "Teste", "Teste", 0.1223);

    ss.saveAllRecordsToTxt("registros_teste.txt");*/
    
    return 0;
}