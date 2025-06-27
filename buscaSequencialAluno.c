#include <stdio.h>
#include <stdlib.h>
#include "aluno.h"

TAluno *buscaSequencialAluno(int matricula, FILE *arquivo, FILE *log) {
    rewind(arquivo); //volta o ponteiro do arquivo pro inicio, ou seja, byte 0
    TAluno *a;
    int pos = 0;
    while ((a = le_aluno(arquivo)) != NULL) {
        //escreve no log qual matrícula está sendo comparada
        fprintf(log, "Comparando com matricula %d\n", a->matricula);
        if (a->matricula == matricula) {
            fprintf(log, "Aluno encontrado na posicao %d\n", pos);
            return a; //retorna o ponteiro pro aluno encontrado (caso ele seja)
        }
        free(a);
        pos++;
    }
    //se o codigo executar ate aqui, é porque leu o arquivo inteiro e nao encontrou, portanto:
    fprintf(log, "Aluno com matricula %d nao encontrado.\n", matricula);
    return NULL;
}
