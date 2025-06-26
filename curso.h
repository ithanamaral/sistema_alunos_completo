#ifndef CURSO_H
#define CURSO_H

#include <stdio.h>

typedef struct {
    int codigo;
    char nome[30];
} TCurso;

void criarBaseCursos(FILE *arquivo, int qtd);
TCurso *le_curso(FILE *arquivo);

#endif