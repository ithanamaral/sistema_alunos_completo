#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aluno.h"
#include <time.h>
#define ARQ_LOG_MERGE "ArquivosLog/logMergeSort.txt"


// Protótipos de funções auxiliares
void merge_files(FILE *arquivo, int l, int m, int r, FILE *temp_file, TAluno *buffer1, TAluno *buffer2, TAluno *output_buffer, int *comparacoes);
void mergeSort_disk(FILE *arquivo, int l, int r, FILE *temp_file, TAluno *buffer1, TAluno *buffer2, TAluno *output_buffer, int *comparacoes);

// Essa função junta dois pedaços do arquivo que já estão ordenados
// Ela compara os elementos e os escreve em ordem no arquivo temporário.
void merge_files(FILE *arquivo, int l, int m, int r, FILE *temp_file,
                 TAluno *buffer1, TAluno *buffer2, TAluno *output_buffer,
                 int *comparacoes) {
    int n1 = m - l + 1;
    int n2 = r - m;

    fseek(arquivo, l * sizeof(TAluno), SEEK_SET);
    fread(buffer1, sizeof(TAluno), n1, arquivo);

    fseek(arquivo, (m + 1) * sizeof(TAluno), SEEK_SET);
    fread(buffer2, sizeof(TAluno), n2, arquivo);

    int i = 0, j = 0, k = 0;

    while (i < n1 && j < n2) {
        (*comparacoes)++; // comparação entre matrículas
        if (buffer1[i].matricula <= buffer2[j].matricula) {
            output_buffer[k++] = buffer1[i++];
        } else {
            output_buffer[k++] = buffer2[j++];
        }
    }

    while (i < n1) {
        output_buffer[k++] = buffer1[i++];
    }
    while (j < n2) {
        output_buffer[k++] = buffer2[j++];
    }

    fseek(arquivo, l * sizeof(TAluno), SEEK_SET);
    fwrite(output_buffer, sizeof(TAluno), (n1 + n2), arquivo);
}


// Essa função divide o arquivo em pedaços menores e chama a merge_files para ordenar
void mergeSort_disk(FILE *arquivo, int l, int r, FILE *temp_file,
                    TAluno *buffer1, TAluno *buffer2, TAluno *output_buffer,
                    int *comparacoes) {
    if (l < r) {
        int m = l + (r - l) / 2;

        mergeSort_disk(arquivo, l, m, temp_file, buffer1, buffer2, output_buffer, comparacoes);
        mergeSort_disk(arquivo, m + 1, r, temp_file, buffer1, buffer2, output_buffer, comparacoes);

        merge_files(arquivo, l, m, r, temp_file, buffer1, buffer2, output_buffer, comparacoes);
    }
}


// Essa função lê todos os alunos do arquivo, ordena por matrícula e grava de novo
void ordenaMergeAlunos(FILE *arquivo) {
    int n = tamanho_arquivo_aluno(arquivo);

    FILE *temp_file = fopen("temp_alunos_sort.dat", "w+b");
    if (temp_file == NULL) {
        perror("Erro ao criar arquivo temporário para ordenação");
        exit(1);
    }

    TAluno *buffer1 = malloc((n / 2 + 1) * sizeof(TAluno));
    TAluno *buffer2 = malloc((n / 2 + 1) * sizeof(TAluno));
    TAluno *output_buffer = malloc(n * sizeof(TAluno));

    if (!buffer1 || !buffer2 || !output_buffer) {
        perror("Erro ao alocar memória para buffers de ordenação");
        fclose(temp_file);
        exit(1);
    }

    rewind(arquivo);

    int comparacoes = 0;
    clock_t inicio = clock();

    mergeSort_disk(arquivo, 0, n - 1, temp_file, buffer1, buffer2, output_buffer, &comparacoes);

    clock_t fim = clock();
    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    FILE *log = fopen("ArquivosLog/logMergeSort.txt", "a");
    if (log) {
        fprintf(log, "\n Comparacoes: %d\n Tempo: %.2fs\n", comparacoes, tempo);
        fclose(log);
    }

    fclose(temp_file);
    remove("temp_alunos_sort.dat");

    free(buffer1);
    free(buffer2);
    free(output_buffer);

    rewind(arquivo);
}

