#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "aluno.h"
#include "curso.h"
#include "ordenacaoAluno.h"
#include "buscaSequencialAluno.h"
#include "buscaBinariaAluno.h"
#include "ordenacaoExternaAluno.h"
#include "tabela_hash_aluno.h"

//Feito por Rafael Araújo e Íthan Amaral

void menu() {
    printf("\n----------- MENU UFOP ------------\n");
    printf("1. Buscar matricula (busca sequencial)\n");
    printf("2. Buscar aluno por nome\n");
    printf("3. Buscar aluno por nome do curso\n");
    printf("4. Buscar matricula (busca binaria)\n");
    printf("5. Exibir a lista de todos os alunos\n");
    printf("6. Ordenar alunos por matricula (modo externo)\n");
    printf("7. Ordenar usando o MergeSort\n");
    printf("8. Acessar Tabela Hash\n");
    printf("9. Sair\n");
    printf("----------------------------------\n");
    printf("Escolha uma opcao: \n ");
}

void menu_hash() {
    printf("\n----------- MENU TABELA HASH ------------\n");
    printf("1. Inserir novo aluno\n");
    printf("2. Buscar aluno por matricula\n");
    printf("3. Remover aluno por matricula (Remocao Logica)\n");
    printf("4. Imprimir estrutura da Tabela Hash\n");
    printf("5. Voltar ao menu principal\n");
    printf("---------------------------------------\n");
    printf("Escolha uma opcao: \n ");
}

int main() {
    setlocale(LC_ALL, "Portuguese");

    FILE *arq, *log, *arquivo_cursos;
    FILE *tabela_hash_dat, *lista_encadeada_dat;

    if ((log = fopen("log_aluno.txt", "a+")) == NULL) { perror("log"); exit(1); }
    if ((arq = fopen("aluno.dat", "w+b")) == NULL) { perror("aluno.dat"); exit(1); }
    if ((arquivo_cursos = fopen("curso.dat", "w+b")) == NULL) { perror("curso.dat"); exit(1); }
    if ((tabela_hash_dat = fopen("tabela_hash_aluno.dat", "w+b")) == NULL) { perror("tabela_hash_aluno.dat"); exit(1); }
    if ((lista_encadeada_dat = fopen("lista_encadeada_aluno.dat", "w+b")) == NULL) { perror("lista_encadeada_aluno.dat"); exit(1); }

    int ordenado = 0;

    int qtdAlunos;
    printf("Digite a quantidade de alunos do ICEA:");
    scanf("%d", &qtdAlunos);

    if (qtdAlunos <= 0) {
        printf("Quantidade invalida de alunos. Encerrando o programa...\n");
        exit(1);
    }

    criarBaseCursos(arquivo_cursos, 5);
    criarBaseAlunos(arq, qtdAlunos);
    //Inicializa a tabela com os alunos da alunos.dat
    inicializa_tabela_hash_alunos(tabela_hash_dat, lista_encadeada_dat, arq);
    printf("\nTabela Hash inicializada com os dados dos alunos.\n");

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
        getchar();

        switch (opcao) {
            case 1: {
                int mat;
                printf("Digite a matricula que voce deseja buscar (busca sequencial): ");
                scanf("%d", &mat);
                TAluno *a = buscaSequencialAluno(mat, arq, log);
                if (a) {
                    imprime_aluno(a, arquivo_cursos);
                    free(a);
                } else {
                    printf("Aluno nao encontrado na base de dados!\n");
                }
                fflush(log);
                break;
            }
            case 2: {
                char nomeBusca[50];
                printf("Digite o nome do aluno: ");
                fgets(nomeBusca, sizeof(nomeBusca), stdin);
                nomeBusca[strcspn(nomeBusca, "\n")] = 0;
                rewind(arq);
                TAluno *a;
                int encontrou = 0;
                while ((a = le_aluno(arq)) != NULL) {
                    if (a->ocupado && strstr(a->nome, nomeBusca)) {
                        imprime_aluno(a, arquivo_cursos);
                        encontrou = 1;
                    }
                    free(a);
                }
                if (!encontrou) printf("Nao ha nenhum aluno com esse nome.\n");
                break;
            }
            case 3: {
                char nomeCurso[30];
                printf("Digite o nome do curso: ");
                fgets(nomeCurso, sizeof(nomeCurso), stdin);
                nomeCurso[strcspn(nomeCurso, "\n")] = 0;

                for(int i = 0; nomeCurso[i]; i++) {
                    nomeCurso[i] = tolower(nomeCurso[i]);
                }

                rewind(arquivo_cursos);
                TCurso *curso;
                int codigosCurso[5] = {0};
                int numCodigos = 0;

                while ((curso = le_curso(arquivo_cursos)) != NULL) {
                    if (strstr(curso->nome, nomeCurso) != NULL) {
                        codigosCurso[numCodigos++] = curso->codigo;
                    }
                    free(curso);
                }

                if (numCodigos == 0) {
                    printf("Nenhum curso encontrado com esse nome.\n");
                    continue;
                }

                rewind(arq);
                TAluno *a;
                int encontrou = 0;
                while ((a = le_aluno(arq)) != NULL) {
                    for (int i = 0; i < numCodigos; i++) {
                        if (a->ocupado && a->curso_codigo == codigosCurso[i]) {
                            imprime_aluno(a, arquivo_cursos);
                            encontrou = 1;
                            break;
                        }
                    }
                    free(a);
                }
                if (!encontrou) printf("Nenhum aluno encontrado nesse curso.\n");
                break;
            }
            case 4: {
                if (!ordenado) {
                    printf("ATENCAO: os alunos nao foram ordenados. A busca binaria pode falhar!\n");
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
                break;
            }
            case 5: {
                rewind(arq);
                TAluno *a;
                printf("Aqui esta a lista de todos os alunos da UFOP:\n");
                while ((a = le_aluno(arq)) != NULL) {
                     if (a->ocupado) {
                        imprime_aluno(a, arquivo_cursos);
                    }
                    free(a);
                }
                break;
            }
            case 6: {
                int m;
                printf("Digite o tamanho maximo da memoria em registros para a Selecao Natural: ");
                scanf("%d", &m);
                clock_t inicioTotal = clock();
                int numParticoes = selecaoNaturalAluno(arq, m);
                intercalacaoOtimaAluno(numParticoes, 3);
                clock_t fimTotal = clock();
                double tempoTotal = (double)(fimTotal - inicioTotal) / CLOCKS_PER_SEC;
                FILE *log_ext = fopen("ArquivosLog/dadosOrdenacaoExterna.txt", "a");
                if (log_ext) {
                    fprintf(log_ext, "Tempo TOTAL de Ordenacao Externa: %.2fs\n\n", tempoTotal);
                    fclose(log_ext);
                }
                printf("Arquivo ordenado externamente com sucesso!\n");
                ordenado = 1;
                break;
            }
            case 7: {
                ordenaMergeAlunos(arq);
                ordenado = 1;
                printf("Mergesort realizado com sucesso.\n");
                break;
            }
            case 8: {
                int opcao_hash;
                do {
                    menu_hash();
                    scanf("%d", &opcao_hash);
                    getchar();

                    switch(opcao_hash) {
                        case 1: {
                            TAluno novo_aluno;
                            printf("Digite a matricula: ");
                            scanf("%d", &novo_aluno.matricula);
                            printf("Digite o nome: ");
                            scanf(" %[^\n]s", novo_aluno.nome);
                            printf("Digite o codigo do curso (1 a 5): ");
                            scanf("%d", &novo_aluno.curso_codigo);
                            novo_aluno.ocupado = true;

                            fseek(arq, 0, SEEK_END);
                            fwrite(&novo_aluno, sizeof(TAluno), 1, arq);
                            fflush(arq);

                            inserir_aluno_hash(tabela_hash_dat, lista_encadeada_dat, &novo_aluno);
                            printf("Aluno inserido com sucesso nos registros e na Tabela Hash.\n");
                            break;
                        }
                        case 2: {
                            int mat;
                            printf("Digite a matricula a buscar: ");
                            scanf("%d", &mat);
                            TAluno* a = buscar_aluno_hash(tabela_hash_dat, lista_encadeada_dat, mat);
                            if (a) {
                                printf("\nAluno encontrado via Tabela Hash:\n");
                                imprime_aluno(a, arquivo_cursos);
                                free(a);
                            } else {
                                printf("Aluno nao encontrado na Tabela Hash.\n");
                            }
                            break;
                        }
                        case 3: {
                            int mat;
                            printf("Digite a matricula a remover: ");
                            scanf("%d", &mat);
                            if (remover_aluno_hash(tabela_hash_dat, lista_encadeada_dat, arq, mat)) {
                                printf("Aluno removido (logicamente) com sucesso.\n");
                            } else {
                                printf("Aluno nao foi encontrado para remocao.\n");
                            }
                            break;
                        }
                        case 4: {
                            imprimir_tabela_hash(tabela_hash_dat, lista_encadeada_dat);
                            break;
                        }
                        case 5:
                            printf("Voltando ao menu principal...\n");
                            break;
                        default:
                            printf("Opcao invalida!\n");
                            break;
                    }
                } while (opcao_hash != 5);
                break;
            }
            case 9:
                break;
            default:
                printf("Opcao invalida!\n");
                break;
        }
        rewind(arq);
    } while (opcao != 9);

    fclose(arq);
    fclose(log);
    fclose(arquivo_cursos);
    fclose(tabela_hash_dat);
    fclose(lista_encadeada_dat);

    printf("Programa encerrado.\n");

    return 0;
}