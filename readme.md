# Projeto Estrutura de Dados - Sequence Set #

#### Integrantes ####
- Este projeto foi desenvolvido pelos alunos:
    1. Gilmar Silva de Medeiros Filho
    2. João Guilherme Santos Ribeiro

#### Objetivo: ####

- Implementar um Sequence Set, uma estrutuda de dados baseada em listas que serve para o gerenciamento eficiente de dados.
- O Sequence Set separa os dados em blocos e mantém um registro com as informações necessárias para manipular os mesmos.

#### Implementação: ####

- Foi implementado um Sequence Set para gerir os dados de taxa de período de vida em diferentes cenários, levando em conta região, faixa etária e forma de medida como principais atributos
- O programa fornece funções de:

   [x] `Inserção via entrada padrão`: permite que o usuário digite no próprio terminal os novos dados a serem adicionados;

   [x] `Inserção via arquivo texto`: permite que o usuário informe o nome de um arquivo texto - que deve estar presente na mesma pasta do programa - que contém os novos dados a serem inseridos;

   [x] `Remoção de dado(s) específico(s)`: permite que o usuário remova um(ou mais) dado(s) específico(s) da base de dados;
          -> É oferecida a opção de remover um registro específico informando os campos: `medida`, `quantil`, `area`, `sexo`, `faixa etária`, `região` e `etnia`
          -> Além disso, também é possível remover todos os registros correspondentes ao informar apenas os campos: `medida`, `faixa etária` e `etnia`

   [x] `Busca de dado(s) específico(s)`: permite que o usuário busque um(ou mais) dado(s) específico(s) na base de dados.
          -> É oferecida a opção de buscar e exibir um registro específico, informando os campos: `medida`, `quantil`, `area`, `sexo`, `faixa etária`, `região` e `etnia`
          -> Além disso, também é possível buscar e exibir todos os dados que forem encontrados ao informar: `medida`, `faixa etária` e `etnia`

   [x] `Salvar os dados em arquivo` : permite que o usuário salve os dado em um arquivo .csv ou .txt, com o nome informado pelo mesmo. # ATENÇÃO #: ao informar o nome do arquivo, não é necessário colocar a extensão!!! Ex.: se você deseja que o arquivo se chame `meu arquivo.txt`, digite no terminal apenas `meu arquivo`.

#### Como executar? ####
No Terminal:
- `Utilize o comando`: make
- `Depois`: make run

#### Também é possível executar de outra maneira ####
Ainda no Terminal:
- `Utilize o comando`: g++ -Wall main.cpp sequenceset.cpp interface.cpp -o app
- `Depois`: ./app