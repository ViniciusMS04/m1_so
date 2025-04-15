// banco.h
#ifndef BANCO_H
#define BANCO_H

#define MAX_REGISTROS 100
#define MAX_MSG 256
#define PIPE_NAME "pipe_banco"

typedef struct {
    int id;
    char nome[50];
} Registro;

typedef struct {
    char comando[10]; // INSERT, DELETE, SELECT, UPDATE
    Registro registro;
} Requisicao;

#endif