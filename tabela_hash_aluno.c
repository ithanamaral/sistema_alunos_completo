// tabela_hash_aluno.c

#ifndef TABELA_HASH_ALUNO_C
#define TABELA_HASH_ALUNO_C

#include <stdlib.h>
#include "tabela_hash_aluno.h"

// Função de Hashing por Divisão
int funcao_hash_aluno(int codigo) {
    return codigo % TAMANHO_HASH_ALUNO;
}

// Inicializa o arquivo da tabela hash com -1 em todas as posições
void cria_tabela_hash_vazia(FILE *tabelaHash) {
    int ponteiroVazio = -1;
    rewind(tabelaHash);
    for (int i = 0; i < TAMANHO_HASH_ALUNO; i++) {
        fwrite(&ponteiroVazio, sizeof(int), 1, tabelaHash);
    }
    fflush(tabelaHash);
}

// Insere um aluno na tabela hash baseada em arquivo
void inserir_aluno_hash(FILE *tabelaHash, FILE *listaEncadeada, TAluno *aluno) {
    int hash_local = funcao_hash_aluno(aluno->matricula);
    fseek(tabelaHash, hash_local * sizeof(int), SEEK_SET);

    int ponteiro_compartimento;
    fread(&ponteiro_compartimento, sizeof(int), 1, tabelaHash);

    TNoHashAluno novo_no;
    novo_no.aluno = *aluno;
    novo_no.proximo = -1;
    novo_no.ocupado = true;

    if (ponteiro_compartimento == -1) {
        // Compartimento vazio, insere na lista e atualiza a tabela
        fseek(listaEncadeada, 0, SEEK_END);
        int pos_novo_no = ftell(listaEncadeada) / sizeof(TNoHashAluno);
        fwrite(&novo_no, sizeof(TNoHashAluno), 1, listaEncadeada);

        fseek(tabelaHash, hash_local * sizeof(int), SEEK_SET);
        fwrite(&pos_novo_no, sizeof(int), 1, tabelaHash);
    } else {
        // Compartimento ocupado, adiciona no fim da lista
        int pos_atual = ponteiro_compartimento;
        TNoHashAluno no_atual;
        while (1) {
            fseek(listaEncadeada, pos_atual * sizeof(TNoHashAluno), SEEK_SET);
            fread(&no_atual, sizeof(TNoHashAluno), 1, listaEncadeada);
            if (no_atual.proximo == -1) {
                break; // Achou o último nó
            }
            pos_atual = no_atual.proximo;
        }

        fseek(listaEncadeada, 0, SEEK_END);
        int pos_novo_no = ftell(listaEncadeada) / sizeof(TNoHashAluno);
        fwrite(&novo_no, sizeof(TNoHashAluno), 1, listaEncadeada);

        // Atualiza o ponteiro do último nó para apontar para o novo
        no_atual.proximo = pos_novo_no;
        fseek(listaEncadeada, pos_atual * sizeof(TNoHashAluno), SEEK_SET);
        fwrite(&no_atual, sizeof(TNoHashAluno), 1, listaEncadeada);
    }
    fflush(tabelaHash);
    fflush(listaEncadeada);
}


// Carrega todos os alunos do arquivo principal para a tabela hash
void inicializa_tabela_hash_alunos(FILE *tabelaHash, FILE *listaEncadeada, FILE *arquivo_alunos) {
    cria_tabela_hash_vazia(tabelaHash);
    rewind(arquivo_alunos);
    TAluno *aluno_lido;
    while ((aluno_lido = le_aluno(arquivo_alunos)) != NULL) {
        if (aluno_lido->ocupado) {
             inserir_aluno_hash(tabelaHash, listaEncadeada, aluno_lido);
        }
        free(aluno_lido);
    }
}


// Busca um aluno na tabela hash
TAluno* buscar_aluno_hash(FILE *tabelaHash, FILE *listaEncadeada, int matricula) {
    int hash_local = funcao_hash_aluno(matricula);
    fseek(tabelaHash, hash_local * sizeof(int), SEEK_SET);

    int ponteiro_compartimento;
    fread(&ponteiro_compartimento, sizeof(int), 1, tabelaHash);

    if (ponteiro_compartimento == -1) {
        return NULL; // Compartimento vazio
    }

    int pos_atual = ponteiro_compartimento;
    while (pos_atual != -1) {
        TNoHashAluno no_atual;
        fseek(listaEncadeada, pos_atual * sizeof(TNoHashAluno), SEEK_SET);
        fread(&no_atual, sizeof(TNoHashAluno), 1, listaEncadeada);

        if (no_atual.aluno.matricula == matricula && no_atual.ocupado) {
            TAluno *aluno_achado = (TAluno*) malloc(sizeof(TAluno));
            *aluno_achado = no_atual.aluno;
            return aluno_achado;
        }
        pos_atual = no_atual.proximo;
    }
    return NULL; // Não encontrou na lista
}

// Realiza a remoção lógica de um aluno
bool remover_aluno_hash(FILE *tabelaHash, FILE *listaEncadeada, FILE *arquivo_alunos, int matricula) {
    int hash_local = funcao_hash_aluno(matricula);
    fseek(tabelaHash, hash_local * sizeof(int), SEEK_SET);

    int ponteiro_compartimento;
    fread(&ponteiro_compartimento, sizeof(int), 1, tabelaHash);

    if (ponteiro_compartimento == -1) {
        return false;
    }

    int pos_atual = ponteiro_compartimento;
    while (pos_atual != -1) {
        TNoHashAluno no_atual;
        fseek(listaEncadeada, pos_atual * sizeof(TNoHashAluno), SEEK_SET);
        fread(&no_atual, sizeof(TNoHashAluno), 1, listaEncadeada);

        if (no_atual.aluno.matricula == matricula && no_atual.ocupado) {
            // Marca como não ocupado na lista encadeada
            no_atual.ocupado = false;
            fseek(listaEncadeada, pos_atual * sizeof(TNoHashAluno), SEEK_SET);
            fwrite(&no_atual, sizeof(TNoHashAluno), 1, listaEncadeada);

            // Marca como não ocupado no arquivo de dados principal (aluno.dat)
            TAluno aluno_arquivo;
            rewind(arquivo_alunos);
            long pos_aluno_arquivo = 0;
            while(fread(&aluno_arquivo, sizeof(TAluno), 1, arquivo_alunos) == 1) {
                if(aluno_arquivo.matricula == matricula) {
                    aluno_arquivo.ocupado = false;
                    fseek(arquivo_alunos, pos_aluno_arquivo * sizeof(TAluno), SEEK_SET);
                    fwrite(&aluno_arquivo, sizeof(TAluno), 1, arquivo_alunos);
                    fflush(arquivo_alunos);
                    break;
                }
                pos_aluno_arquivo++;
            }
            return true;
        }
        pos_atual = no_atual.proximo;
    }
    return false; // Não encontrou
}

// Imprime a estrutura da tabela hash
void imprimir_tabela_hash(FILE *tabelaHash, FILE *listaEncadeada) {
    rewind(tabelaHash);
    printf("\n--- ESTRUTURA DA TABELA HASH (ALUNOS) ---\n");

    for (int i = 0; i < TAMANHO_HASH_ALUNO; i++) {
        int ponteiro;
        fread(&ponteiro, sizeof(int), 1, tabelaHash);

        printf("\nCompartimento [%03d]: ", i);
        if (ponteiro == -1) {
            printf("Vazio.");
        } else {
            printf("Ponteiro para indice %d da lista.", ponteiro);
            int pos_atual = ponteiro;
            while (pos_atual != -1) {
                TNoHashAluno no_atual;
                fseek(listaEncadeada, pos_atual * sizeof(TNoHashAluno), SEEK_SET);
                fread(&no_atual, sizeof(TNoHashAluno), 1, listaEncadeada);

                if (no_atual.ocupado) {
                    printf("\n    -> [Mat: %d, Nome: %s, Proximo_Indice: %d]",
                           no_atual.aluno.matricula, no_atual.aluno.nome, no_atual.proximo);
                }
                pos_atual = no_atual.proximo;
            }
        }
        printf("\n-----------------------------------------\n");
    }
}

#endif