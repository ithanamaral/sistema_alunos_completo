#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curso.h"

void criarBaseCursos(FILE *arquivo, int qtd) {
    char nomes[5][30] = {"Computacao", "Direito", "Engenharia", "Medicina", "Arquitetura"};
    for (int i = 0; i < qtd; i++) {
        TCurso curso;
        curso.codigo = i + 1;
        strncpy(curso.nome, nomes[i % 5], sizeof(curso.nome));
        fwrite(&curso, sizeof(TCurso), 1, arquivo);
    }
    rewind(arquivo);
}

TCurso *le_curso(FILE *arquivo) {
    TCurso *curso = (TCurso *) malloc(sizeof(TCurso));
    if (fread(curso, sizeof(TCurso), 1, arquivo) != 1) {
        free(curso);
        return NULL;
    }
    return curso;
}
