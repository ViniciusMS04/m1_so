// cliente.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "banco.h"

void enviar_requisicao(const char *comando, int id, const char *nome) {
    int fd = open(PIPE_NAME, O_WRONLY);
    if (fd < 0) {
        perror("Erro ao abrir pipe");
        exit(1);
    }

    Requisicao req;
    memset(&req, 0, sizeof(req));
    strncpy(req.comando, comando, sizeof(req.comando) - 1);
    req.registro.id = id;
    strncpy(req.registro.nome, nome, sizeof(req.registro.nome) - 1);

    write(fd, &req, sizeof(req));
    close(fd);
}

int main() {
    int opcao, id;
    char nome[50];

    while (1) {
        printf("\n=== MENU ===\n");
        printf("1. Inserir\n");
        printf("2. Remover\n");
        printf("3. Buscar (SELECT)\n");
        printf("4. Atualizar (UPDATE)\n");
        printf("5. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar();

        if (opcao == 5) break;

        printf("Digite o ID: ");
        scanf("%d", &id);
        getchar();

        if (opcao == 1 || opcao == 4) {
            printf("Digite o nome: ");
            fgets(nome, sizeof(nome), stdin);
            nome[strcspn(nome, "\n")] = '\0';
        } else {
            strcpy(nome, "");
        }

        switch (opcao) {
            case 1:
                enviar_requisicao("INSERT", id, nome);
                break;
            case 2:
                enviar_requisicao("DELETE", id, "");
                break;
            case 3:
                enviar_requisicao("SELECT", id, "");
                break;
            case 4:
                enviar_requisicao("UPDATE", id, nome);
                break;
            default:
                printf("Opcao invalida!\n");
        }
    }

    return 0;
}