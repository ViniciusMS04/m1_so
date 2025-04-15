void* processar_requisicao(void* arg) {
    Requisicao *req = (Requisicao*) arg;

    pthread_mutex_lock(&mutex);

 
    req->comando[9] = '\0';

    if (strncmp(req->comando, "INSERT", 6) == 0) {
        banco[total_registros++] = req->registro;
        printf("[INSERT] %d %s inserido\n", req->registro.id, req->registro.nome);
    } 
    else if (strncmp(req->comando, "DELETE", 6) == 0) {
        for (int i = 0; i < total_registros; i++) {
            if (banco[i].id == req->registro.id) {
                printf("[DELETE] %d %s removido\n", banco[i].id, banco[i].nome);
                banco[i] = banco[--total_registros];
                break;
            }
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
            printf("[SELECT] Registro com ID %d não encontrado.\n", req->registro.id);
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
            printf("[UPDATE] Registro com ID %d não encontrado.\n", req->registro.id);
        }
    }

    salvar_banco();
    pthread_mutex_unlock(&mutex);
    free(req);
    return NULL;
}
