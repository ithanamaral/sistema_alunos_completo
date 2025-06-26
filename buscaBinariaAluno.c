#include <stdio.h>
#include <stdlib.h>
#include "aluno.h"

TAluno *busca_binaria_aluno(int matricula, FILE *arquivo, int ini, int fim, FILE *log) {
    if (ini > fim) {
        fprintf(log, "Aluno com matricula %d nao encontrado.\n", matricula);
        return NULL;
    }

    int meio = (ini + fim) / 2;
    fseek(arquivo, meio * sizeof(TAluno), SEEK_SET);
    TAluno *a = le_aluno(arquivo);

    if (!a) return NULL;

    fprintf(log, "Comparando com matricula %d\n", a->matricula);

    if (a->matricula == matricula) {
        fprintf(log, "Aluno encontrado na posicao %d\n", meio);
        return a;
    }

    if (matricula < a->matricula) {
        free(a);
        return busca_binaria_aluno(matricula, arquivo, ini, meio - 1, log);
    } else {
        free(a);
        return busca_binaria_aluno(matricula, arquivo, meio + 1, fim, log);
    }
}
