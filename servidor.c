// servidor.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include "banco.h"

Registro banco[MAX_REGISTROS];
int total_registros = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void salvar_banco() {
    FILE *fp = fopen("banco.txt", "w");
    for (int i = 0; i < total_registros; i++) {
        fprintf(fp, "%d %s\n", banco[i].id, banco[i].nome);
    }
    fclose(fp);
}

void* processar_requisicao(void* arg) {
    Requisicao *req = (Requisicao*) arg;

    pthread_mutex_lock(&mutex);

    req->comando[9] = '\0';

    if (strncmp(req->comando, "INSERT", 6) == 0) {
        banco[total_registros++] = req->registro;
        printf("[INSERT] %d %s inserido\n", req->registro.id, req->registro.nome);
    } 
    else if (strncmp(req->comando, "DELETE", 6) == 0) {
        int removido = 0;
        for (int i = 0; i < total_registros; i++) {
            if (banco[i].id == req->registro.id) {
                printf("[DELETE] %d %s removido\n", banco[i].id, banco[i].nome);
                banco[i] = banco[--total_registros];
                removido = 1;
                break;
            }
        }
        if (!removido) {
            printf("[DELETE] Registro com ID %d nao encontrado.\n", req->registro.id);
        }
    }
    else if (strncmp(req->comando, "SELECT", 6) == 0) {
        int encontrado = 0;
        for (int i = 0; i < total_registros; i++) {
            if (banco[i].id == req->registro.id) {
                printf("[SELECT] Encontrado: %d %s\n", banco[i].id, banco[i].nome);
                encontrado = 1;
                break;
            }
        }
        if (!encontrado) {
            printf("[SELECT] Registro com ID %d nao encontrado.\n", req->registro.id);
        }
    }
    else if (strncmp(req->comando, "UPDATE", 6) == 0) {
        int atualizado = 0;
        for (int i = 0; i < total_registros; i++) {
            if (banco[i].id == req->registro.id) {
                printf("[UPDATE] %d %s -> %s\n", banco[i].id, banco[i].nome, req->registro.nome);
                strncpy(banco[i].nome, req->registro.nome, sizeof(banco[i].nome));
                atualizado = 1;
                break;
            }
        }
        if (!atualizado) {
            printf("[UPDATE] Registro com ID %d nao encontrado.\n", req->registro.id);
        }
    }

    salvar_banco();

    pthread_mutex_unlock(&mutex);
    free(req);
    return NULL;
}

int main() {
    mkfifo(PIPE_NAME, 0666);
    int fd = open(PIPE_NAME, O_RDONLY);
    if (fd < 0) {
        perror("Erro ao abrir pipe");
        exit(1);
    }

    while (1) {
        Requisicao *req = malloc(sizeof(Requisicao));
        if (read(fd, req, sizeof(Requisicao)) > 0) {
            pthread_t thread;
            pthread_create(&thread, NULL, processar_requisicao, req);
            pthread_detach(thread);
        } else {
            free(req);
        }
    }

    close(fd);
    unlink(PIPE_NAME);
    return 0;
}
