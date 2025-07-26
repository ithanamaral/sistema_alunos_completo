#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "aluno.h"
#include "ordenacaoExternaAluno.h"

#define DIR_PARTICOES "Ordenacao/Particoes"
#define ARQ_LOG "ArquivosLog/dadosOrdenacaoExterna.txt"

// Funcoes auxiliares para heap
void trocaAlunos(TAluno *a, TAluno *b) {
    TAluno tmp = *a;
    *a = *b;
    *b = tmp;
}

void heapifyAluno(TAluno *arr, int n, int i) {
    int menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if (esq < n && arr[esq].matricula < arr[menor].matricula)
        menor = esq;
    if (dir < n && arr[dir].matricula < arr[menor].matricula)
        menor = dir;

    if (menor != i) {
        trocaAlunos(&arr[i], &arr[menor]);
        heapifyAluno(arr, n, menor);
    }
}

int selecaoNaturalAluno(FILE *entrada, int M) {
    entrada = fopen("aluno.dat", "rb");
    if (!entrada) {
        perror("Erro ao abrir aluno.dat");
        return -1;
    }

    clock_t inicio = clock();
    int comparacoes = 0;

    TAluno memoria[M];
    TAluno reserva[M];
    int tamMem = 0, tamRes = 0;
    int fim = 0;
    int particao = 1;
    char nome[100];

#ifdef _WIN32
    system("mkdir Ordenacao\\Particoes > nul 2>&1");
    system("mkdir ArquivosLog > nul 2>&1");
#else
    system("mkdir -p Ordenacao/Particoes");
    system("mkdir -p ArquivosLog");
#endif


    for (int i = 0; i < M; i++) {
        if (fread(&memoria[tamMem], sizeof(TAluno), 1, entrada) == 1) {
            tamMem++;
        } else {
            fim = 1;
            break;
        }
    }

    for (int i = tamMem / 2 - 1; i >= 0; i--) heapifyAluno(memoria, tamMem, i);

    sprintf(nome, DIR_PARTICOES "/particao_%d.dat", particao);
    FILE *saida = fopen(nome, "wb");
    printf("\n------ Selecao Natural ------\n");
    printf("Gerando particao %d:\n", particao);


    while (!fim || tamMem > 0) {
        if (tamMem > 0) {
            TAluno menor = memoria[0];
            fwrite(&menor, sizeof(TAluno), 1, saida);
            printf("%d ", menor.matricula);

            TAluno novo;
            if (!fim && fread(&novo, sizeof(TAluno), 1, entrada) == 1) {
                comparacoes++;
                if (novo.matricula >= menor.matricula) {
                    memoria[0] = novo;
                } else {
                    reserva[tamRes++] = novo;
                    memoria[0] = memoria[tamMem - 1];
                    tamMem--;
                }
            } else {
                fim = 1;
                memoria[0] = memoria[tamMem - 1];
                tamMem--;
            }
            heapifyAluno(memoria, tamMem, 0);
        }

        if (tamMem == 0 && tamRes > 0) {
            fclose(saida);
            printf("\n");
            particao++;
            sprintf(nome, DIR_PARTICOES "/particao_%d.dat", particao);
            saida = fopen(nome, "wb");
            printf("Gerando particao %d:\n", particao);
            for (int i = 0; i < tamRes; i++) memoria[i] = reserva[i];
            tamMem = tamRes;
            tamRes = 0;
            for (int i = tamMem / 2 - 1; i >= 0; i--) heapifyAluno(memoria, tamMem, i);
        }
    }

    fclose(saida);
    fclose(entrada);
    printf("\n");

    clock_t fimExec = clock();
    double tempo = (double)(fimExec - inicio) / CLOCKS_PER_SEC;

    FILE *log = fopen(ARQ_LOG, "a");
    if (log) {
        fprintf(log, "\nSelecao Natural - Particoes: %d, Comparacoes: %d, Tempo: %.2fs\n", particao, comparacoes, tempo);
        fclose(log);
    }

    return particao;
}

// Struct auxiliar para intercalacao
typedef struct {
    TAluno aluno;
    int origem;
} HeapAluno;

void heapifyHeapAluno(HeapAluno *arr, int n, int i, int *comparacoes) {
    int menor = i;
    int esq = 2 * i + 1;
    int dir = 2 * i + 2;

    if (esq < n && arr[esq].aluno.matricula < arr[menor].aluno.matricula) menor = esq;
    if (dir < n && arr[dir].aluno.matricula < arr[menor].aluno.matricula) menor = dir;
    (*comparacoes) += 2;

    if (menor != i) {
        HeapAluno tmp = arr[i];
        arr[i] = arr[menor];
        arr[menor] = tmp;
        heapifyHeapAluno(arr, n, menor, comparacoes);
    }
}
//Intercalacao Otima
void intercalacaoOtimaAluno(int numParticoes, int F) {
    clock_t inicio = clock();
    int comparacoes = 0;

    printf("\n------ Intercalacao Otima ------\n");

    char arquivos[numParticoes][100];
    for (int i = 0; i < numParticoes; i++) {
        sprintf(arquivos[i], DIR_PARTICOES "/particao_%d.dat", i + 1);
    }

    int rodada = 1;
    int atual = numParticoes;
    char novos[100][100];

    while (atual > 1) {
        int novoQtd = 0;
        printf("Rodada %d:\n", rodada);

        for (int i = 0; i < atual; i += F) {
            int fim = (i + F < atual) ? (i + F) : atual;
            char nomeSaida[100];
            sprintf(nomeSaida, DIR_PARTICOES "/intercalado_%d.dat", rodada++);
            FILE *saida = fopen(nomeSaida, "wb");
            FILE *entradas[F];
            HeapAluno heap[F];
            int tam = 0;

            printf("  Mesclando ");
            for (int j = i; j < fim; j++) {
                printf("'%s' ", arquivos[j]);
                entradas[j - i] = fopen(arquivos[j], "rb");
                if (fread(&heap[tam].aluno, sizeof(TAluno), 1, entradas[j - i]) == 1) {
                    heap[tam].origem = j - i;
                    tam++;
                }
            }printf("para '%s':\n    ", nomeSaida);

            for (int j = tam / 2 - 1; j >= 0; j--)
                heapifyHeapAluno(heap, tam, j, &comparacoes);

            while (tam > 0) {
                fwrite(&heap[0].aluno, sizeof(TAluno), 1, saida);
                printf("%d ", heap[0].aluno.matricula);
                if (fread(&heap[0].aluno, sizeof(TAluno), 1, entradas[heap[0].origem]) != 1) {
                    heap[0] = heap[tam - 1];
                    tam--;
                }
                heapifyHeapAluno(heap, tam, 0, &comparacoes);
            }
            printf("\n");

            for (int j = 0; j < fim - i; j++) fclose(entradas[j]);
            fclose(saida);
            strcpy(novos[novoQtd++], nomeSaida);
        }

        for (int i = 0; i < novoQtd; i++) strcpy(arquivos[i], novos[i]);
        atual = novoQtd;
    }

    //copia final para aluno.dat
    FILE *final = fopen("aluno.dat", "wb");
    FILE *intercalado = fopen(arquivos[0], "rb");
    TAluno a;
    printf("\nCopiando resultado final para aluno.dat:\n");
    while (fread(&a, sizeof(TAluno), 1, intercalado) == 1) {
        fwrite(&a, sizeof(TAluno), 1, final);
        printf("%d ", a.matricula);
    }
    printf("\n");
    fclose(intercalado);
    fclose(final);

    clock_t fimExec = clock();
    double tempo = (double)(fimExec - inicio) / CLOCKS_PER_SEC;

    FILE *log = fopen(ARQ_LOG, "a");
    if (log) {
        fprintf(log, "Intercalacao Otima - Comparacoes: %d, Tempo: %.2fs\n", comparacoes, tempo);
        fclose(log);
    }
}