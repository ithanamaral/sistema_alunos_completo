#ifndef TABELA_HASH_ALUNO_H
#define TABELA_HASH_ALUNO_H

#include <stdbool.h>
#include "aluno.h"

// Tamanho da tabela hash. Um número primo é geralmente uma boa escolha.
#define TAMANHO_HASH_ALUNO 101

// Estrutura do nó que será salva no arquivo de lista encadeada
typedef struct {
    TAluno aluno;
    int proximo;
    bool ocupado;
} TNoHashAluno;


int funcao_hash_aluno(int codigo);
void cria_tabela_hash_vazia(FILE *tabelaHash);
void inicializa_tabela_hash_alunos(FILE *tabelaHash, FILE *listaEncadeada, FILE *arquivo_alunos);
void inserir_aluno_hash(FILE *tabelaHash, FILE *listaEncadeada, TAluno *aluno);
TAluno* buscar_aluno_hash(FILE *tabelaHash, FILE *listaEncadeada, int matricula);
bool remover_aluno_hash(FILE *tabelaHash, FILE *listaEncadeada, FILE *arquivo_alunos, int matricula);
void imprimir_tabela_hash(FILE *tabelaHash, FILE *listaEncadeada);

#endif

