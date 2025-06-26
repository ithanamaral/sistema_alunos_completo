#ifndef BUSCA_BINARIA_ALUNO_H
#define BUSCA_BINARIA_ALUNO_H

#include <stdio.h>
#include "aluno.h"

TAluno *busca_binaria_aluno(int matricula, FILE *arquivo, int ini, int fim, FILE *log);

#endif