#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "aluno.h"
#include "curso.h"
#include "ordenacaoAluno.h"
#include "buscaSequencialAluno.h"
#include "buscaBinariaAluno.h"
#include "ordenacaoExternaAluno.h"

//Feito por Rafael Araújo e Íthan Amaral

void menu() {
    printf("\n----------- MENU UFOP ------------\n");
    printf("1. Buscar matricula (busca sequencial)\n");
    printf("2. Buscar aluno por nome\n");
    printf("3. Buscar aluno por nome do curso\n");
    printf("4. Buscar matricula (busca binaria)\n");
    printf("5. Exibir a lista de todos os alunos\n");
    printf("6. Ordenar alunos por matricula (modo externo)\n");
    printf("7. Sair\n");
    printf("----------------------------------\n");
    printf("Escolha uma opcao: \n ");
}

int main() {
    setlocale(LC_ALL, "Portuguese");

    FILE *arq, *log, *arquivo_cursos;

    //O fopen é usado pra abrir ou criar os arquivos log e binários e exibe mensagens de erro se o comando falhar
    if ((log = fopen("log_aluno.txt", "a+")) == NULL) { perror("log"); exit(1); }
    if ((arq = fopen("aluno.dat", "w+b")) == NULL) { perror("aluno.dat"); exit(1); }
    if ((arquivo_cursos = fopen("curso.dat", "w+b")) == NULL) { perror("curso.dat"); exit(1); }

    int ordenado = 0; // inicializa como não ordenado

    int qtdAlunos;
    printf("Digite a quantidade de alunos do ICEA:");
    scanf("%d", &qtdAlunos);

    if (qtdAlunos <= 0) {
        printf("Quantidade invalida de alunos. Encerrando o programa...\n");
        exit(1);
    }

    //Inicializa os dados
    criarBaseCursos(arquivo_cursos, 5); // mantém os cursos fixos disponíveis no ICEA
    criarBaseAlunos(arq, qtdAlunos); //alunos definidos pelo usuário

    char resposta;
    printf("Deseja ordenar os alunos por matricula agora? \n(Por favor, responda com S ou N): ");
    scanf(" %c", &resposta);

    if (resposta == 's' || resposta == 'S') {
        ordenaMergeAlunos(arq);
        ordenado = 1;

        printf("Alunos ordenados com sucesso.\n");
    } else {
        printf("Os alunos nao foram ordenados. A busca binaria pode nao funcionar corretamente!\n");
    }

        int opcao;
        do {
            menu();
            scanf("%d", &opcao);
            getchar(); // consome o \n

            if (opcao == 1) {
                int mat;
                printf("Digite a matricula que voce deseja buscar (busca sequencial): ");
                scanf("%d", &mat);
                TAluno *a = buscaSequencialAluno(mat, arq, log); //procura a matricula no arquivo
                if (a) {
                    imprime_aluno(a, arquivo_cursos);
                    free(a);
                } else {
                    printf("Aluno nao encontrado na base de dados!\n");
                }
                fflush(log);

            } else if (opcao == 2) {
                char nomeBusca[50];
                printf("Digite o nome do aluno: ");
                fgets(nomeBusca, sizeof(nomeBusca), stdin);
                nomeBusca[strcspn(nomeBusca, "\n")] = 0; // remove \n
                rewind(arq); //reposiciona o ponteiro do arquivo pro inicio
                TAluno *a;
                int encontrou = 0;

                while ((a = le_aluno(arq)) != NULL) {
                    if (strstr(a->nome, nomeBusca)) { //procura a substring na string maior de nomes
                        imprime_aluno(a, arquivo_cursos);
                        encontrou = 1;
                    }
                    free(a);
                }
                if (!encontrou) printf("Não ha nenhum aluno com esse nome.\n");

            } else if (opcao == 3) {
                char nomeCurso[30];
                printf("Digite o nome do curso: ");
                fgets(nomeCurso, sizeof(nomeCurso), stdin);
                nomeCurso[strcspn(nomeCurso, "\n")] = 0; // remove \n

                for(int i = 0; nomeCurso[i]; i++) {
                    nomeCurso[i] = tolower(nomeCurso[i]); //converte os caracteres para letras minúsculas
                }

                rewind(arquivo_cursos);
                TCurso *curso;
                int codigosCurso[5] = {0}; // para armazenar múltiplos códigos que correspondam
                int numCodigos = 0;

                while ((curso = le_curso(arquivo_cursos)) != NULL) {
                    if (strstr(curso->nome, nomeCurso) != NULL) {
                        codigosCurso[numCodigos++] = curso->codigo;
                        printf("Curso encontrado: %s (codigo: %d)\n", curso->nome, curso->codigo);
                    }
                    free(curso);
                }

                if (numCodigos == 0) {
                    printf("Nenhum curso encontrado com esse nome.\n");
                    continue;
                }

                //Agora buscamos os alunos
                rewind(arq);
                TAluno *a;
                int encontrou = 0;

                while ((a = le_aluno(arq)) != NULL) {
                    for (int i = 0; i < numCodigos; i++) {
                        if (a->curso_codigo == codigosCurso[i]) {
                            imprime_aluno(a, arquivo_cursos);
                            encontrou = 1;
                            break; //pois já encontrou correspondência
                        }
                    }
                    free(a);
                }

                if (!encontrou) printf("Nenhum aluno encontrado nesse curso.\n");

            } else if (opcao == 4) {
                if (!ordenado) {
                    printf("ATENCAO: os alunos nao foram ordenados. A busca binaria pode falhar!\n");
                    printf("E recomendado que voce reinicie o programa e confirme a ordenacao no inicio.\n");
                    continue;
                }


                int mat;
                printf("Digite a matricula a buscar (busca binaria): ");
                scanf("%d", &mat);
                int total = tamanho_arquivo_aluno(arq);
                TAluno *a = busca_binaria_aluno(mat, arq, 0, total - 1, log);
                if (a) {
                    imprime_aluno(a, arquivo_cursos);
                    free(a);
                } else {
                    printf("Aluno nao encontrado.\n");
                }
                fflush(log);

            } else if (opcao == 5) {
                rewind(arq);
                TAluno *a;
                printf("Aqui esta a lista de todos os alunos da UFOP:\n");

                while ((a = le_aluno(arq)) != NULL) {
                    imprime_aluno(a, arquivo_cursos);
                    free(a);
                }

            }else if (opcao == 6) {
                int m;
                printf("Digite o tamanho maximo da memoria em registros para a Selecao Natural: ");
                scanf("%d", &m);
                int numParticoes = selecaoNaturalAluno(arq, m);
                intercalacaoOtimaAluno(numParticoes, 3); // ou outro valor de F
                printf("Arquivo ordenado externamente com sucesso!\n");
                ordenado = 1;

            }else if (opcao != 7) {
                printf("Opcao invalida!\n");
            }

            rewind(arq); // volta pro inicio do arquivo

        } while (opcao != 7);

        fclose(arq);
        fclose(log);
        fclose(arquivo_cursos);
        return 0;
}
