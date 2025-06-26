#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "aluno.h"
#include "curso.h"
#include "ordenacaoAluno.h"
#include "buscaSequencialAluno.h"
#include "buscaBinariaAluno.h"

//Feito por Rafael Araújo e Íthan Amaral

void menu() {
    printf("\n----------- MENU UFOP ------------\n");
    printf("1. Buscar matricula (busca sequencial)\n");
    printf("2. Buscar aluno por nome\n");
    printf("3. Buscar aluno por nome do curso\n");
    printf("4. Buscar matricula (busca binaria)\n");
    printf("5. Sair\n");
    printf("Escolha uma opcao: ");
}

int main() {

    setlocale(LC_ALL, "Portuguese");

    FILE *arq, *log, *arquivo_cursos;

    if ((log = fopen("log_aluno.txt", "a+")) == NULL) { perror("log"); exit(1); }
    if ((arq = fopen("aluno.dat", "w+b")) == NULL) { perror("aluno.dat"); exit(1); }
    if ((arquivo_cursos = fopen("curso.dat", "w+b")) == NULL) { perror("curso.dat"); exit(1); }

    // inicializando dados
    criarBaseCursos(arquivo_cursos, 5);
    criarBaseAlunos(arq, 1000);
    ordenaMergeAlunos(arq);

    int opcao;
    do {
        menu();
        scanf("%d", &opcao);
        getchar(); // consumir \n

        if (opcao == 1) {
            int mat;
            printf("Digite a matricula que voce deseja buscar (sequencial): ");
            scanf("%d", &mat);
            TAluno *a = buscaSequencialAluno(mat, arq, log);
            if (a) {
                imprime_aluno(a, arquivo_cursos);
                free(a);
            } else {
                printf("Aluno nao encontrado na base de dados!\n");
            }

        } else if (opcao == 2) {
            char nomeBusca[50];
            printf("Digite o nome do aluno: ");
            fgets(nomeBusca, sizeof(nomeBusca), stdin);
            nomeBusca[strcspn(nomeBusca, "\n")] = 0; // remove \n
            rewind(arq);
            TAluno *a;
            int encontrou = 0;

            while ((a = le_aluno(arq)) != NULL) {
                if (strstr(a->nome, nomeBusca)) {
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
            
            // Primeiro, vamos encontrar o código do curso pelo nome
            rewind(arquivo_cursos);
            TCurso *curso;
            int codigosCurso[5] = {0}; // Para armazenar múltiplos códigos que correspondam
            int numCodigos = 0;
            
            while ((curso = le_curso(arquivo_cursos)) != NULL) {
                if (strstr(curso->nome, nomeCurso) != NULL) {
                    codigosCurso[numCodigos++] = curso->codigo;
                    printf("Curso encontrado: %s (código: %d)\n", curso->nome, curso->codigo);
                }
                free(curso);
            }
            
            if (numCodigos == 0) {
                printf("Nenhum curso encontrado com esse nome.\n");
                continue;
            }
            
            // Agora busca os alunos com os códigos dos cursos encontrados
            rewind(arq);
            TAluno *a;
            int encontrou = 0;
            
            while ((a = le_aluno(arq)) != NULL) {
                for (int i = 0; i < numCodigos; i++) {
                    if (a->curso_codigo == codigosCurso[i]) {
                        imprime_aluno(a, arquivo_cursos);
                        encontrou = 1;
                        break; // Já encontrou correspondência, não precisa verificar outros códigos
                    }
                }
                free(a);
            }
            
            if (!encontrou) printf("Nenhum aluno encontrado nesse curso.\n");

        } else if (opcao == 4) {
            int mat;
            printf("Digite a matricula a buscar (binária): ");
            scanf("%d", &mat);
            int total = tamanho_arquivo_aluno(arq);
            TAluno *a = busca_binaria_aluno(mat, arq, 0, total - 1, log);
            if (a) {
                imprime_aluno(a, arquivo_cursos);
                free(a);
            } else {
                printf("Aluno nao encontrado.\n");
            }

        } else if (opcao != 5) {
            printf("Opcao invalida!\n");
        }

        rewind(arq);

    } while (opcao != 5);

    fclose(arq);
    fclose(log);
    fclose(arquivo_cursos);
    return 0;
}
