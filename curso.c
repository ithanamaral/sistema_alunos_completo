#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curso.h"

void criarBaseCursos(FILE *arquivo, int qtd) {
    //array com os cursos do ICEA
    char nomes[4][30] = {
        "engenharia de computacao",
        "engenharia eletrica",
        "sistemas",
        "engenharia de producao",
    };

    //grava os cursos diretamente no arquivo
    for (int i = 0; i < qtd; i++) {
        TCurso curso;
        curso.codigo = i + 1;
        strncpy(curso.nome, nomes[i % 4], sizeof(curso.nome));
        
        //posiciona o ponteiro do arquivo na posição correta
        fseek(arquivo, i * sizeof(TCurso), SEEK_SET);
        //grava o curso no arquivo
        fwrite(&curso, sizeof(TCurso), 1, arquivo);
    }
    rewind(arquivo);
}

TCurso *le_curso(FILE *arquivo) {
    TCurso *curso = (TCurso *) malloc(sizeof(TCurso));
    
    //tenta ler um curso do arquivo
    if (fread(curso, sizeof(TCurso), 1, arquivo) != 1) {
        free(curso);
        return NULL;
    }
    return curso;
}

