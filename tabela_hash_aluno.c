#ifndef TABELA_HASH_ALUNO_C
#define TABELA_HASH_ALUNO_C

#include <stdlib.h>
#include "tabela_hash_aluno.h"

// função de hashing por divisão
// ela vai calcular a posição do aluno na tabela
int funcao_hash_aluno(int codigo) {
    return codigo % TAMANHO_HASH_ALUNO;
}

// inicializa o arquivo da tabela hash com -1 em todas as posições
void cria_tabela_hash_vazia(FILE *tabelaHash) {
    int ponteiroVazio = -1; // -1 atua como um ponteiro nulo, indicando que o compartimento da tabela está vazio
    rewind(tabelaHash);
    // percorre a tabela inteira, escrevendo -1 em cada posição.
    for (int i = 0; i < TAMANHO_HASH_ALUNO; i++) {
        fwrite(&ponteiroVazio, sizeof(int), 1, tabelaHash);
    }
    fflush(tabelaHash);
}

//insere um aluno na tabela hash baseada em arquivo
void inserir_aluno_hash(FILE *tabelaHash, FILE *listaEncadeada, TAluno *aluno) {
    // primeiro, calcula a posição (hash) onde o aluno deveria estar
    int hash_local = funcao_hash_aluno(aluno->matricula);
    fseek(tabelaHash, hash_local * sizeof(int), SEEK_SET);

    // lê o valor que está nesse local. esse valor é o ponteiro para o primeiro nó da lista encadeada.
    int ponteiro_compartimento;
    fread(&ponteiro_compartimento, sizeof(int), 1, tabelaHash);

    // prepara o novo nó que será inserido na lista, com os dados do aluno.
    TNoHashAluno novo_no;
    novo_no.aluno = *aluno;
    novo_no.proximo = -1; // como ele será o último da lista, o próximo é -1.
    novo_no.ocupado = true; // marca o nó atual como ocupado.

    // verifica se o compartimento (bucket) está vazio.
    if (ponteiro_compartimento == -1) {
        // se estiver vazio, a inserção é mais simples.
        // vai para o final do arquivo da lista encadeada para adicionar o novo nó.
        fseek(listaEncadeada, 0, SEEK_END);
        // calcula a posição (índice) do novo nó.
        //ftell  retorna a posição atual do "cursor", até o final, em bytes informando o tamanho do arquivo
        //sizeoff calcula o tamanho do registro
        int pos_novo_no = ftell(listaEncadeada) / sizeof(TNoHashAluno); //número total de registros já existentes
        fwrite(&novo_no, sizeof(TNoHashAluno), 1, listaEncadeada);

        // agora, atualiza a tabela hash para apontar para este novo nó.
        fseek(tabelaHash, hash_local * sizeof(int), SEEK_SET);
        fwrite(&pos_novo_no, sizeof(int), 1, tabelaHash);
    } else {
        // se o compartimento já estiver ocupado, significa que houve uma colisão.
        // precisamos adicionar o novo aluno ao final da lista encadeada existente.
        int pos_atual = ponteiro_compartimento;
        TNoHashAluno no_atual;
        // percorre a lista até encontrar o último nó (aquele cujo 'proximo' é -1).
        while (1) {
            fseek(listaEncadeada, pos_atual * sizeof(TNoHashAluno), SEEK_SET);
            fread(&no_atual, sizeof(TNoHashAluno), 1, listaEncadeada);
            if (no_atual.proximo == -1) {
                break; // achou o último nó.
            }
            pos_atual = no_atual.proximo;
        }

        // agora que achamos o fim da lista, adiciona o novo nó no final do arquivo.
        fseek(listaEncadeada, 0, SEEK_END);
        int pos_novo_no = ftell(listaEncadeada) / sizeof(TNoHashAluno);
        fwrite(&novo_no, sizeof(TNoHashAluno), 1, listaEncadeada);

        // por fim, atualiza o antigo último nó para que ele aponte para o novo nó que acabamos de inserir.
        no_atual.proximo = pos_novo_no;
        fseek(listaEncadeada, pos_atual * sizeof(TNoHashAluno), SEEK_SET);
        fwrite(&no_atual, sizeof(TNoHashAluno), 1, listaEncadeada);
    }
    // garante que todas as alterações sejam salvas nos arquivos.
    fflush(tabelaHash);
    fflush(listaEncadeada);
}


// carrega todos os alunos do arquivo principal aluno.dat para a tabela hash
void inicializa_tabela_hash_alunos(FILE *tabelaHash, FILE *listaEncadeada, FILE *arquivo_alunos) {
    cria_tabela_hash_vazia(tabelaHash); // primeiro, limpa a tabela hash.
    rewind(arquivo_alunos); // volta para o início do arquivo de alunos.
    TAluno *aluno_lido;
    // lê cada aluno do arquivo principal.
    // lê um registro de aluno, aloca memória e retorna um ponteiro (aluno_lido) para essa estrutura.
    while ((aluno_lido = le_aluno(arquivo_alunos)) != NULL) {
        // insere na tabela hash apenas se o aluno estiver marcado como 'ocupado'.
        if (aluno_lido->ocupado) {
             inserir_aluno_hash(tabelaHash, listaEncadeada, aluno_lido);
        }
        free(aluno_lido); // libera a memória usada para ler o aluno.
    }
}

// busca por um aluno usando a matrícula, retornando o struct de aluno.
TAluno* buscar_aluno_hash(FILE *tabelaHash, FILE *listaEncadeada, int matricula) {
    // calcula a posição provável do aluno na tabela.
    int hash_local = funcao_hash_aluno(matricula);
    // Manda o ponteiro para essa posição
    fseek(tabelaHash, hash_local * sizeof(int), SEEK_SET);

    // lê o ponteiro para a lista de alunos naquela posição.
    int ponteiro_compartimento;
    fread(&ponteiro_compartimento, sizeof(int), 1, tabelaHash);

    // se o ponteiro é -1, o compartimento está vazio, então o aluno não existe.
    if (ponteiro_compartimento == -1) {
        return NULL; // compartimento vazio.
    }

    // se chegou aqui, existe uma lista. vamos percorrê-la.
    int pos_atual = ponteiro_compartimento;
    while (pos_atual != -1) {
        TNoHashAluno no_atual;
        fseek(listaEncadeada, pos_atual * sizeof(TNoHashAluno), SEEK_SET);
        fread(&no_atual, sizeof(TNoHashAluno), 1, listaEncadeada);

        // verifica se a matrícula do nó atual é a que procuramos e se ele está 'ocupado'.
        if (no_atual.aluno.matricula == matricula && no_atual.ocupado) {
            // se encontrou, aloca memória para o aluno e retorna uma cópia dos seus dados.
            TAluno *aluno_achado = (TAluno*) malloc(sizeof(TAluno));
            *aluno_achado = no_atual.aluno;
            return aluno_achado;
        }
        // se não for este, vai para o próximo da lista.
        pos_atual = no_atual.proximo;
    }
    return NULL; // não encontrou na lista.
}
// remove um aluno logicamente, marcando como nao ocupado
bool remover_aluno_hash(FILE *tabelaHash, FILE *listaEncadeada, FILE *arquivo_alunos, int matricula) {
    // o processo de busca inicial é o mesmo da função de buscar.
    int hash_local = funcao_hash_aluno(matricula);
    fseek(tabelaHash, hash_local * sizeof(int), SEEK_SET);

    int ponteiro_compartimento;
    fread(&ponteiro_compartimento, sizeof(int), 1, tabelaHash);

    if (ponteiro_compartimento == -1) {
        return false; // o aluno não está aqui.
    }

    //inicia a navegacao pela lista encadeada
    int pos_atual = ponteiro_compartimento;
    while (pos_atual != -1) {
        TNoHashAluno no_atual;
        fseek(listaEncadeada, pos_atual * sizeof(TNoHashAluno), SEEK_SET);
        fread(&no_atual, sizeof(TNoHashAluno), 1, listaEncadeada);

        if (no_atual.aluno.matricula == matricula && no_atual.ocupado) {
            // encontrou o aluno. agora vamos marcá-lo como removido.

            // marca como não ocupado no arquivo da lista encadeada.
            no_atual.ocupado = false;
            fseek(listaEncadeada, pos_atual * sizeof(TNoHashAluno), SEEK_SET);
            fwrite(&no_atual, sizeof(TNoHashAluno), 1, listaEncadeada);

            // marca não ocupado no arquivo de dados de alunos tambem
            TAluno aluno_arquivo;
            rewind(arquivo_alunos);
            long pos_aluno_arquivo = 0;
            // procura o aluno no arquivo principal.
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
            return true; // sucesso na remoção.
        }
        pos_atual = no_atual.proximo;
    }
    return false; // não encontrou o aluno pra remover
}

// função para visualizar a estrutura da tabela hash pra apresentacao
void imprimir_tabela_hash(FILE *tabelaHash, FILE *listaEncadeada) {
    rewind(tabelaHash);
    printf("\n--- estrutura da tabela hash (alunos) ---\n");

    // percorre cada compartimento da tabela.
    for (int i = 0; i < TAMANHO_HASH_ALUNO; i++) {
        int ponteiro;
        fread(&ponteiro, sizeof(int), 1, tabelaHash);

        printf("\ncompartimento [%03d]: ", i);
        if (ponteiro == -1) {
            printf("vazio.");
        } else {
            // se não estiver vazio, mostra para qual índice da lista ele aponta.
            printf("ponteiro para indice %d da lista.", ponteiro);
            int pos_atual = ponteiro;
            // percorre e imprime toda a lista encadeada daquele compartimento.
            while (pos_atual != -1) {
                TNoHashAluno no_atual;
                fseek(listaEncadeada, pos_atual * sizeof(TNoHashAluno), SEEK_SET);
                fread(&no_atual, sizeof(TNoHashAluno), 1, listaEncadeada);

                if (no_atual.ocupado) {
                    printf("\n    -> [mat: %d, nome: %s, proximo_indice: %d]",
                           no_atual.aluno.matricula, no_atual.aluno.nome, no_atual.proximo);
                }
                pos_atual = no_atual.proximo;
            }
        }
        printf("\n-----------------------------------------\n");
    }
}

#endif