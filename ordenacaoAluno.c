#include <stdio.h>
#include <stdlib.h>
#include "aluno.h"

//Essa função junta dois pedaços do vetor que já estão ordenados
//ela compara os elementos e coloca em ordem no vetor original
void merge(TAluno *arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    //cria dois vetores temporários para guardar as metades
    TAluno *L = malloc(n1 * sizeof(TAluno));
    TAluno *R = malloc(n2 * sizeof(TAluno));

    //copia os dados das duas partes para os vetores temporários
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    //agora compara os elementos dos dois vetores e coloca em ordem no original
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i].matricula <= R[j].matricula)
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }
    //se sobrar elementos nos int L ou R, eles são colocados diretamente no vetor original
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    free(L);
    free(R);
}

//Essa função divide o vetor em pedaços menores e chama a merge pra ordenar
void mergeSort(TAluno *arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2; //calcula o meio do vetor
        mergeSort(arr, l, m); //ordena a primeira parte
        mergeSort(arr, m + 1, r); //ordena a segunda
        merge(arr, l, m, r); //junta tudo
    }
}

//Essa função lê todos os alunos do arquivo, ordena por matrícula e grava denovo
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
