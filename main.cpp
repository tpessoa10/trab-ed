#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORDEM 3  // Ordem da árvore B+ (máximo 2 chaves por nó)

// Registro de cliente
typedef struct {
    int codigo;
    char nome[50];
} Cliente;

// Nó da árvore B+
typedef struct NoBMais {
    int folha;
    int num_chaves;
    int chaves[ORDEM - 1];
    int rrns[ORDEM - 1]; // Só se for folha
    struct NoBMais *filhos[ORDEM];
    struct NoBMais *prox; // Liga folhas
} NoBMais;

int salvarCliente(Cliente c) {
    FILE *arquivo = fopen("clientes.dat", "ab");
    if (!arquivo) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }

    fseek(arquivo, 0, SEEK_END);
    long pos = ftell(arquivo);
    int rrn = pos / sizeof(Cliente);
    fwrite(&c, sizeof(Cliente), 1, arquivo);
    fclose(arquivo);
    return rrn;
}

void buscarClientePorRRN(int rrn) {
    FILE *arquivo = fopen("clientes.dat", "rb");
    if (!arquivo) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }

    Cliente c;
    fseek(arquivo, rrn * sizeof(Cliente), SEEK_SET);
    fread(&c, sizeof(Cliente), 1, arquivo);
    printf("Código: %d | Nome: %s\n", c.codigo, c.nome);

    fclose(arquivo);
}

NoBMais* criarNo(int folha) {
    NoBMais *no = (NoBMais*)malloc(sizeof(NoBMais));
    no->folha = folha;
    no->num_chaves = 0;
    no->prox = NULL;
    for (int i = 0; i < ORDEM; i++) no->filhos[i] = NULL;
    return no;
}

NoBMais* inserirNaFolha(NoBMais* raiz, int chave, int rrn) {
    int i = 0;
    while (i < raiz->num_chaves && chave > raiz->chaves[i]) i++;

    for (int j = raiz->num_chaves; j > i; j--) {
        raiz->chaves[j] = raiz->chaves[j - 1];
        raiz->rrns[j] = raiz->rrns[j - 1];
    }

    raiz->chaves[i] = chave;
    raiz->rrns[i] = rrn;
    raiz->num_chaves++;
    return raiz;
}

NoBMais* inserir(NoBMais* raiz, int chave, int rrn) {
    if (raiz == NULL) {
        raiz = criarNo(1);
        raiz->chaves[0] = chave;
        raiz->rrns[0] = rrn;
        raiz->num_chaves = 1;
        return raiz;
    }

    if (raiz->num_chaves < ORDEM - 1) {
        return inserirNaFolha(raiz, chave, rrn);
    }

    // Simples: dividir nó folha cheio
    NoBMais* novaFolha = criarNo(1);
    int tempChaves[ORDEM];
    int tempRRNs[ORDEM];
    
    for (int i = 0; i < ORDEM - 1; i++) {
        tempChaves[i] = raiz->chaves[i];
        tempRRNs[i] = raiz->rrns[i];
    }
    tempChaves[ORDEM - 1] = chave;
    tempRRNs[ORDEM - 1] = rrn;

    // Ordenar
    for (int i = 0; i < ORDEM - 1; i++) {
        for (int j = i + 1; j < ORDEM; j++) {
            if (tempChaves[i] > tempChaves[j]) {
                int t = tempChaves[i]; tempChaves[i] = tempChaves[j]; tempChaves[j] = t;
                t = tempRRNs[i]; tempRRNs[i] = tempRRNs[j]; tempRRNs[j] = t;
            }
        }
    }

    raiz->num_chaves = ORDEM / 2;
    novaFolha->num_chaves = ORDEM - ORDEM / 2;

    for (int i = 0; i < raiz->num_chaves; i++) {
        raiz->chaves[i] = tempChaves[i];
        raiz->rrns[i] = tempRRNs[i];
    }
    for (int i = 0; i < novaFolha->num_chaves; i++) {
        novaFolha->chaves[i] = tempChaves[i + raiz->num_chaves];
        novaFolha->rrns[i] = tempRRNs[i + raiz->num_chaves];
    }

    raiz->prox = novaFolha;

    // Retornamos raiz (em sistema completo, teria um nó pai)
    return raiz;
}

int buscarRRN(NoBMais* raiz, int codigo) {
     NoBMais* atual = raiz;

    // Vai até a primeira folha
    while (!atual->folha)
        atual = atual->filhos[0];

    // Percorre as folhas encadeadas
    while (atual) {
        for (int i = 0; i < atual->num_chaves; i++) {
            if (atual->chaves[i] == codigo)
                return atual->rrns[i];
        }
        atual = atual->prox;
    }

    return -1; 
}

void imprimirOrdem(NoBMais* raiz) {
    NoBMais* atual = raiz;
    while (!atual->folha) atual = atual->filhos[0];
    while (atual) {
        for (int i = 0; i < atual->num_chaves; i++) {
            buscarClientePorRRN(atual->rrns[i]);
        }
        atual = atual->prox;
    }
}

int main() {
    NoBMais* raiz = NULL;

    Cliente c1 = {1001, "Maria Silva"};
    Cliente c2 = {1005, "João Souza"};
    Cliente c3 = {1003, "Carlos Lima"};
    Cliente c4 = {1007, "Carlas Lima"};

    int r1 = salvarCliente(c1);
    raiz = inserir(raiz, c1.codigo, r1);

    int r4 = salvarCliente(c4);
    raiz = inserir(raiz, c4.codigo, r4);

    int r2 = salvarCliente(c2);
    raiz = inserir(raiz, c2.codigo, r2);

    int r3 = salvarCliente(c3);
    raiz = inserir(raiz, c3.codigo, r3);

    printf("Busca pelo código 1003:\n");
    int rrnBusca = buscarRRN(raiz, 1003);
    if (rrnBusca != -1) buscarClientePorRRN(rrnBusca);

    printf("Busca pelo código 1007:\n");
    int rrnBusca1 = buscarRRN(raiz, 1007);
    if (rrnBusca1 != -1) buscarClientePorRRN(rrnBusca1);

    printf("\nListando em ordem:\n");
    imprimirOrdem(raiz);
    

    return 0;
}