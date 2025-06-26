#include <stdio.h>
#include <stdlib.h>
#include "aluno.h"

TAluno *buscaSequencialAluno(int matricula, FILE *arquivo, FILE *log) {
    rewind(arquivo);
    TAluno *a;
    int pos = 0;
    while ((a = le_aluno(arquivo)) != NULL) {
        fprintf(log, "Comparando com matricula %d\n", a->matricula);
        if (a->matricula == matricula) {
            fprintf(log, "Aluno encontrado na posicao %d\n", pos);
            return a;
        }
        free(a);
        pos++;
    }
    fprintf(log, "Aluno com matricula %d nao encontrado.\n", matricula);
    return NULL;
}
