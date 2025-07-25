#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aluno.h"

// Protótipos de funções auxiliares
void merge_files(FILE *arquivo, int l, int m, int r, FILE *temp_file, TAluno *buffer1, TAluno *buffer2, TAluno *output_buffer);
void mergeSort_disk(FILE *arquivo, int l, int r, FILE *temp_file, TAluno *buffer1, TAluno *buffer2, TAluno *output_buffer);

// Essa função junta dois pedaços do arquivo que já estão ordenados
// Ela compara os elementos e os escreve em ordem no arquivo temporário.
void merge_files(FILE *arquivo, int l, int m, int r, FILE *temp_file, TAluno *buffer1, TAluno *buffer2, TAluno *output_buffer) {
    int n1 = m - l + 1;
    int n2 = r - m;

    // Reposiciona o ponteiro do arquivo principal para a primeira parte
    fseek(arquivo, l * sizeof(TAluno), SEEK_SET);
    // Lê a primeira metade para o buffer1
    fread(buffer1, sizeof(TAluno), n1, arquivo);

    // Reposiciona o ponteiro do arquivo principal para a segunda parte
    fseek(arquivo, (m + 1) * sizeof(TAluno), SEEK_SET);
    // Lê a segunda metade para o buffer2
    fread(buffer2, sizeof(TAluno), n2, arquivo);

    int i = 0, j = 0, k = 0;

    // Compara os elementos dos dois buffers e os coloca em ordem no buffer de saida
    while (i < n1 && j < n2) {
        if (buffer1[i].matricula <= buffer2[j].matricula) {
            output_buffer[k++] = buffer1[i++];
        } else {
            output_buffer[k++] = buffer2[j++];
        }
    }

    // Se sobrarem elementos no buffer 1, eles são colocados diretamente no buffer de saida
    while (i < n1) {
        output_buffer[k++] = buffer1[i++];
    }
    // Se sobrarem elementos no buffer 2, eles são colocados diretamente no buffer de saida
    while (j < n2) {
        output_buffer[k++] = buffer2[j++];
    }

    // Escreve o output_buffer de volta no arquivo principal na posição correta
    fseek(arquivo, l * sizeof(TAluno), SEEK_SET);
    fwrite(output_buffer, sizeof(TAluno), (n1 + n2), arquivo);
}

// Essa função divide o arquivo em pedaços menores e chama a merge_files para ordenar
void mergeSort_disk(FILE *arquivo, int l, int r, FILE *temp_file, TAluno *buffer1, TAluno *buffer2, TAluno *output_buffer) {
    if (l < r) {
        int m = l + (r - l) / 2; // Calcula o meio do arquivo
        mergeSort_disk(arquivo, l, m, temp_file, buffer1, buffer2, output_buffer); // Ordena a primeira parte
        mergeSort_disk(arquivo, m + 1, r, temp_file, buffer1, buffer2, output_buffer); // Ordena a segunda
        merge_files(arquivo, l, m, r, temp_file, buffer1, buffer2, output_buffer); // Junta tudo
    }
}

// Essa função lê todos os alunos do arquivo, ordena por matrícula e grava de novo
void ordenaMergeAlunos(FILE *arquivo) {
    int n = tamanho_arquivo_aluno(arquivo);

    // Cria um arquivo temporário para auxiliar na ordenação.
    // Usamos "w+b" para criar um arquivo binário para leitura e escrita.
    FILE *temp_file = fopen("temp_alunos_sort.dat", "w+b");
    if (temp_file == NULL) {
        perror("Erro ao criar arquivo temporário para ordenação");
        exit(1);
    }

    // Aloca buffers para leitura e escrita, para evitar muitas operações de I/O diretas.
    // O tamanho dos buffers pode ser ajustado para otimização.
    TAluno *buffer1 = malloc((n / 2 + 1) * sizeof(TAluno));
    TAluno *buffer2 = malloc((n / 2 + 1) * sizeof(TAluno));
    TAluno *output_buffer = malloc(n * sizeof(TAluno));

    if (!buffer1 || !buffer2 || !output_buffer) {
        perror("Erro ao alocar memória para buffers de ordenação");
        fclose(temp_file);
        exit(1);
    }

    rewind(arquivo); // Volta para o início do arquivo principal

    // Inicia a ordenação no disco
    mergeSort_disk(arquivo, 0, n - 1, temp_file, buffer1, buffer2, output_buffer);

    // Fecha e remove o arquivo temporário
    fclose(temp_file);
    remove("temp_alunos_sort.dat");

    free(buffer1);
    free(buffer2);
    free(output_buffer);

    rewind(arquivo); // Volta para o início do arquivo principal após a ordenação
}
