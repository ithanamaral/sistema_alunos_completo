#include <stdio.h>
#include <stdlib.h>
#include "aluno.h"

void merge(TAluno *arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    TAluno *L = malloc(n1 * sizeof(TAluno));
    TAluno *R = malloc(n2 * sizeof(TAluno));

    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i].matricula <= R[j].matricula)
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(TAluno *arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void ordenaMergeAlunos(FILE *arquivo) {
    int n = tamanho_arquivo_aluno(arquivo);
    TAluno *arr = malloc(n * sizeof(TAluno));
    rewind(arquivo);
    fread(arr, sizeof(TAluno), n, arquivo);

    mergeSort(arr, 0, n - 1);

    rewind(arquivo);
    fwrite(arr, sizeof(TAluno), n, arquivo);
    rewind(arquivo);
    free(arr);
}
