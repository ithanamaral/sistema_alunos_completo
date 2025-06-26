#ifndef ALUNO_H
#define ALUNO_H

#include <stdio.h>

typedef struct {
    int matricula;
    char nome[50];
    int curso_codigo;
} TAluno;

void criarBaseAlunos(FILE *arquivo, int qtd);
TAluno *le_aluno(FILE *arquivo);
void imprime_aluno(TAluno *a, FILE *arquivo_cursos);
int tamanho_arquivo_aluno(FILE *arquivo);

#endif
