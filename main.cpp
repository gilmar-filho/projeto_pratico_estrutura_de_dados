#include <iostream>
#include <stdexcept>
#include "data.h"

using namespace std;

int main(){
    SequenceSet ss;
    try {
        SequenceSet ss;
        ss.carregarArquivo("Subnational-period-life-tables-2017-2019-CSV.csv", false);
        int opcao;
        do {
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
                ss.carregarArquivo(nomeArqTxt,true);
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
                string chaveMedida, chaveIdade, chaveEtnia;
                cout << "Informe a medida para buscar: ";
                cin >> chaveMedida;
                cin.ignore();
                cout << "Informe a idade para buscar: ";
                getline(cin, chaveIdade);
                cout << "Informe a etnia para buscar: ";
                cin >> chaveEtnia;

                ss.buscarRegistro(chaveMedida, chaveIdade, chaveEtnia);
                break;
            }
            case 5: {
                string nomeArqTxt;
                cout << "Informe o nome do arquivo texto: ";
                cin >> nomeArqTxt;
                ss.salvarEmTxt(nomeArqTxt);
                break;
            }
            case 0:
                ss.salvarEmTxt("registros.txt");
                cout << "Saindo...\n";
                break;
            default:
                cout << "Opção inválida!\n";
            }
        } while (opcao != 0);
    } catch (const exception &e) {
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