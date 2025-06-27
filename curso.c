#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curso.h"

void criarBaseCursos(FILE *arquivo, int qtd) {
    //array com os cursos do ICEA
    char nomes[4][30] = {
        "Engenharia de Computacao",
        "Engenharia Eletrica",
        "Sistemas",
        "Engenharia de Producao",
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

//função auxiliar para obter o tamanho do arquivo de cursos
int tamanho_arquivo_curso(FILE *arquivo) {
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    rewind(arquivo);
    return (int)(tamanho / sizeof(TCurso));
}