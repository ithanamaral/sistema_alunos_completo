cmake_minimum_required(VERSION 3.31)
project(sistema_alunos_completo C)

set(CMAKE_C_STANDARD 11)

if(MSVC)
    add_compile_options(/wd4996)
else()
    add_compile_options(-Wno-format -Wno-deprecated-declarations)
endif()
if(MSVC)
    add_compile_options(/utf-8)
endif()


add_executable(sistema_alunos_completo
        aluno.c
        aluno.h
        buscaBinariaAluno.c
        buscaBinariaAluno.h
        buscaSequencialAluno.c
        buscaSequencialAluno.h
        curso.c
        curso.h
        main.c
        ordenacaoAluno.c
        ordenacaoAluno.h
        ordenacaoExternaAluno.c
        ordenacaoExternaAluno.h)
