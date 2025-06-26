#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aluno.h"
#include "curso.h"

void criarBaseAlunos(FILE *arquivo, int qtd) {
    srand(time(NULL));

    // Gera e grava alunos com matrícula sequencial com for
    for (int i = 0; i < qtd; i++) {
        TAluno aluno;
        aluno.matricula = 100 + i; // sequencial (Evita repeticao)
        sprintf(aluno.nome, "Aluno %d", i + 1);
        aluno.curso_codigo = 1 + rand() % 5;

        fwrite(&aluno, sizeof(TAluno), 1, arquivo);
    }

    // Embaralha os registros diretamente no arquivo para evitar usar a memoria RAM
    for (int i = qtd - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        TAluno aluno_i, aluno_j;

        // lê aluno i
        fseek(arquivo, i * sizeof(TAluno), SEEK_SET);
        fread(&aluno_i, sizeof(TAluno), 1, arquivo);

        // lê aluno j
        fseek(arquivo, j * sizeof(TAluno), SEEK_SET);
        fread(&aluno_j, sizeof(TAluno), 1, arquivo);

        // grava aluno_j na posição i
        fseek(arquivo, i * sizeof(TAluno), SEEK_SET);
        fwrite(&aluno_j, sizeof(TAluno), 1, arquivo);

        // grava aluno_i na posição j
        fseek(arquivo, j * sizeof(TAluno), SEEK_SET);
        fwrite(&aluno_i, sizeof(TAluno), 1, arquivo);
    }

    rewind(arquivo); // volta pro início
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
