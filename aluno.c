#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aluno.h"

void criarBaseAlunos(FILE *arquivo, int qtd) {
    for (int i = 0; i < qtd; i++) {
        TAluno aluno;
        aluno.matricula = 100 + i;
        sprintf(aluno.nome, "Aluno %d", i + 1);
        aluno.curso_codigo = 1 + rand() % 5;
        fwrite(&aluno, sizeof(TAluno), 1, arquivo);
    }
    rewind(arquivo);
}

TAluno *le_aluno(FILE *arquivo) {
    TAluno *aluno = (TAluno *) malloc(sizeof(TAluno));
    if (fread(aluno, sizeof(TAluno), 1, arquivo) != 1) {
        free(aluno);
        return NULL;
    }
    return aluno;
}

void imprime_aluno(TAluno *a) {
    printf("Matricula: %d\nNome: %s\nCurso Codigo: %d\n\n",
           a->matricula, a->nome, a->curso_codigo);
}

int tamanho_arquivo_aluno(FILE *arquivo) {
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    rewind(arquivo);
    return (int)(tamanho / sizeof(TAluno));
}
