#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aluno.h"
#include "curso.h"
#include "ordenacaoAluno.h"
#include "buscaSequencialAluno.h"
#include "buscaBinariaAluno.h"

void menu() {
    printf("\n==== MENU ====\n");
    printf("1. Buscar por matrícula (sequencial)\n");
    printf("2. Buscar por nome\n");
    printf("3. Buscar por curso\n");
    printf("4. Buscar por matrícula (binária)\n");
    printf("5. Sair\n");
    printf("Escolha uma opção: ");
}

int main() {
    FILE *arq, *log, *arquivo_cursos;

    if ((log = fopen("log_aluno.txt", "a+")) == NULL) { perror("log"); exit(1); }
    if ((arq = fopen("aluno.dat", "w+b")) == NULL) { perror("aluno.dat"); exit(1); }
    if ((arquivo_cursos = fopen("curso.dat", "w+b")) == NULL) { perror("curso.dat"); exit(1); }

    // Inicializar dados
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
            printf("Digite a matrícula a buscar (sequencial): ");
            scanf("%d", &mat);
            TAluno *a = buscaSequencialAluno(mat, arq, log);
            if (a) {
                imprime_aluno(a);
                free(a);
            } else {
                printf("Aluno não encontrado.\n");
            }

        } else if (opcao == 2) {
            char nomeBusca[50];
            printf("Digite parte do nome do aluno: ");
            fgets(nomeBusca, sizeof(nomeBusca), stdin);
            nomeBusca[strcspn(nomeBusca, "\n")] = 0; // remove \n
            rewind(arq);
            TAluno *a;
            int encontrou = 0;
            while ((a = le_aluno(arq)) != NULL) {
                if (strstr(a->nome, nomeBusca)) {
                    imprime_aluno(a);
                    encontrou = 1;
                }
                free(a);
            }
            if (!encontrou) printf("Nenhum aluno com esse nome.\n");

        } else if (opcao == 3) {
            int codigoCurso;
            printf("Digite o código do curso: ");
            scanf("%d", &codigoCurso);
            rewind(arq);
            TAluno *a;
            int encontrou = 0;
            while ((a = le_aluno(arq)) != NULL) {
                if (a->curso_codigo == codigoCurso) {
                    imprime_aluno(a);
                    encontrou = 1;
                }
                free(a);
            }
            if (!encontrou) printf("Nenhum aluno encontrado nesse curso.\n");

        } else if (opcao == 4) {
            int mat;
            printf("Digite a matrícula a buscar (binária): ");
            scanf("%d", &mat);
            int total = tamanho_arquivo_aluno(arq);
            TAluno *a = busca_binaria_aluno(mat, arq, 0, total - 1, log);
            if (a) {
                imprime_aluno(a);
                free(a);
            } else {
                printf("Aluno não encontrado.\n");
            }

        } else if (opcao != 5) {
            printf("Opção inválida!\n");
        }

        rewind(arq);

    } while (opcao != 5);

    fclose(arq);
    fclose(log);
    fclose(arquivo_cursos);
    return 0;
}
