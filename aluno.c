#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aluno.h"
#include "curso.h"

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

void imprime_aluno(TAluno *a, FILE *arquivo_cursos) {
    // Salvar a posição atual do arquivo de cursos
    long posicao_atual = ftell(arquivo_cursos);
    
    // Voltar para o início do arquivo
    rewind(arquivo_cursos);
    
    // Procurar o curso com o código correspondente
    TCurso *curso;
    char nome_curso[30] = "Curso não encontrado";
    
    while ((curso = le_curso(arquivo_cursos)) != NULL) {
        if (curso->codigo == a->curso_codigo) {
            strcpy(nome_curso, curso->nome);
            free(curso);
            break;
        }
        free(curso);
    }
    
    // Imprimir informações do aluno com o nome do curso
    printf("Matricula: %d\nNome: %s\nCurso: %s\n\n",
           a->matricula, a->nome, nome_curso);
    
    // Restaurar a posição do arquivo de cursos
    fseek(arquivo_cursos, posicao_atual, SEEK_SET);
}

int tamanho_arquivo_aluno(FILE *arquivo) {
    fseek(arquivo, 0, SEEK_END);
    long tamanho = ftell(arquivo);
    rewind(arquivo);
    return (int)(tamanho / sizeof(TAluno));
}
