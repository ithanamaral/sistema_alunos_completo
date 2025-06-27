#include <stdio.h>
#include <stdlib.h>
#include "aluno.h"

//Essa função vai realizar busca binaria em um arquivo binario de alunos
TAluno *busca_binaria_aluno(int matricula, FILE *arquivo, int ini, int fim, FILE *log) {

    //se o intervalo é inválido, o aluno não foi encontrado
    if (ini > fim) {
        fprintf(log, "Aluno com matricula %d nao encontrado.\n", matricula);
        return NULL;
    }

    int meio = (ini + fim) / 2;
    fseek(arquivo, meio * sizeof(TAluno), SEEK_SET);
    TAluno *a = le_aluno(arquivo);

    if (!a) return NULL;

    //escreve no log qual matrícula está sendo comparada
    fprintf(log, "Comparando com matricula %d\n", a->matricula);

    if (a->matricula == matricula) {
        fprintf(log, "Aluno encontrado na posicao %d\n", meio);
        return a;
    }

    //se a matrícula procurada for menor, a funcao ontinua buscando na outra metade
    if (matricula < a->matricula) {
        free(a);
        return busca_binaria_aluno(matricula, arquivo, ini, meio - 1, log);
    } else {
        //se a matrícula procurada for maior, continua buscando na metade superior
        free(a);
        return busca_binaria_aluno(matricula, arquivo, meio + 1, fim, log);
    }
}
