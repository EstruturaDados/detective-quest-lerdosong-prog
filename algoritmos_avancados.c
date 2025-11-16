#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SALA_NOME 30
#define MAX_PISTA_TEXTO 100
#define TAMANHO_HASH 5

typedef struct Sala {
    char nome[MAX_SALA_NOME];
    char pistaAssociada[MAX_PISTA_TEXTO];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

typedef struct PistaNode {
    char texto[MAX_PISTA_TEXTO];
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

typedef struct Associacao {
    char pista[MAX_PISTA_TEXTO];
    struct Associacao *proximo;
} Associacao;

typedef struct Suspeito {
    char nome[MAX_SALA_NOME];
    int contagemPistas;
    Associacao *listaPistas;
    struct Suspeito *proximo;
} Suspeito;

Suspeito *tabelaHash[TAMANHO_HASH];

void limparBufferEntrada();
Sala* criarSala(const char *nome, const char *pista);
void liberarMapa(Sala *raiz);
void liberarPistas(PistaNode *raiz);
void liberarHash();

Sala* construirMapa();
void explorarSalas(Sala *raiz, PistaNode **arvorePistas);

PistaNode* inserirBST(PistaNode *raiz, const char *texto);
void emOrdem(PistaNode *raiz);
void listarPistas(PistaNode *raiz);

void inicializarHash();
unsigned int calcularHash(const char *nome);
void inserirHash(const char *suspeitoNome, const char *pistaTexto);
void listarAssociacoes();
void encontrarSuspeitoMaisProvavel();
Suspeito* buscarSuspeito(const char *nome);

int main() {
    Sala *mapaRaiz = construirMapa();
    PistaNode *arvorePistas = NULL;

    if (mapaRaiz == NULL) {
        printf("Erro ao construir o mapa.\n");
        return 1;
    }
    
    inicializarHash();

    printf("--- Detective Quest: O Misterio da Mansao ---\n");
    printf("Bem-vindo, Detetive. Voce esta no Hall de Entrada.\n");
    
    explorarSalas(mapaRaiz, &arvorePistas);

    printf("\n\n--- Fim da Investigacao ---\n");
    listarPistas(arvorePistas);
    printf("\n");
    listarAssociacoes();
    printf("\n");
    encontrarSuspeitoMaisProvavel();

    liberarMapa(mapaRaiz);
    liberarPistas(arvorePistas);
    liberarHash();
    
    return 0;
}

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*)malloc(sizeof(Sala));
    if (nova == NULL) {
        perror("Erro de alocacao para Sala");
        return NULL;
    }
    strncpy(nova->nome, nome, MAX_SALA_NOME - 1);
    nova->nome[MAX_SALA_NOME - 1] = '\0';
    strncpy(nova->pistaAssociada, pista, MAX_PISTA_TEXTO - 1);
    nova->pistaAssociada[MAX_PISTA_TEXTO - 1] = '\0';
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

void liberarMapa(Sala *raiz) {
    if (raiz != NULL) {
        liberarMapa(raiz->esquerda);
        liberarMapa(raiz->direita);
        free(raiz);
    }
}

void liberarPistas(PistaNode *raiz) {
    if (raiz != NULL) {
        liberarPistas(raiz->esquerda);
        liberarPistas(raiz->direita);
        free(raiz);
    }
}

Sala* construirMapa() {
    Sala *hall = criarSala("Hall de Entrada", "O porteiro viu um carro vermelho.");

    Sala *biblioteca = criarSala("Biblioteca", "Um bilhete rasgado sob o sofa.");
    Sala *cozinha = criarSala("Cozinha", "Facas organizadas, exceto uma.");
    hall->esquerda = biblioteca;
    hall->direita = cozinha;

    biblioteca->esquerda = criarSala("Escritorio", "O Abajur esta ligado, ha impressao digital.");
    biblioteca->direita = criarSala("Quarto Principal", "Um relogio parado as 3:00.");
    cozinha->esquerda = criarSala("Dispensa", "A porta da dispensa foi forcada.");
    cozinha->direita = criarSala("Sotao", "Um mapa incompleto do bairro.");

    return hall;
}

void explorarSalas(Sala *raiz, PistaNode **arvorePistas) {
    Sala *atual = raiz;
    char acao;
    bool pistaColetada = false;

    while (atual != NULL) {
        if (!pistaColetada && strlen(atual->pistaAssociada) > 0) {
            printf("\n--- PISTA ENCONTRADA ---\n");
            printf("Pista: %s\n", atual->pistaAssociada);
            *arvorePistas = inserirBST(*arvorePistas, atual->pistaAssociada);
            
            if (strcmp(atual->nome, "Biblioteca") == 0) {
                 inserirHash("Mordomo", atual->pistaAssociada);
            } else if (strcmp(atual->nome, "Cozinha") == 0) {
                 inserirHash("Chef", atual->pistaAssociada);
            } else {
                 inserirHash("Vizinho", atual->pistaAssociada);
            }
            
            pistaColetada = true;
        }

        printf("\nVoce esta em: %s\n", atual->nome);
        printf("Acoes - (e) Esquerda, (d) Direita, (s) Sair/Finalizar: ");
        
        if (scanf(" %c", &acao) != 1) {
            acao = 's';
        }
        limparBufferEntrada();
        pistaColetada = false;

        if (acao == 's' || acao == 'S') {
            printf("Saindo da sala...\n");
            break;
        } else if ((acao == 'e' || acao == 'E') && atual->esquerda != NULL) {
            atual = atual->esquerda;
        } else if ((acao == 'd' || acao == 'D') && atual->direita != NULL) {
            atual = atual->direita;
        } else {
            printf("Nao ha saida nessa direcao. Tente outra acao.\n");
        }
    }
}

PistaNode* inserirBST(PistaNode *raiz, const char *texto) {
    if (raiz == NULL) {
        PistaNode *nova = (PistaNode*)malloc(sizeof(PistaNode));
        if (nova == NULL) return NULL;
        strncpy(nova->texto, texto, MAX_PISTA_TEXTO - 1);
        nova->texto[MAX_PISTA_TEXTO - 1] = '\0';
        nova->esquerda = nova->direita = NULL;
        return nova;
    }

    int comparacao = strcmp(texto, raiz->texto);

    if (comparacao < 0) {
        raiz->esquerda = inserirBST(raiz->esquerda, texto);
    } else if (comparacao > 0) {
        raiz->direita = inserirBST(raiz->direita, texto);
    }
    return raiz;
}

void emOrdem(PistaNode *raiz) {
    if (raiz != NULL) {
        emOrdem(raiz->esquerda);
        printf("- %s\n", raiz->texto);
        emOrdem(raiz->direita);
    }
}

void listarPistas(PistaNode *raiz) {
    printf("--- Evidencias Coletadas (Em Ordem Alfabetica) ---\n");
    if (raiz == NULL) {
        printf("Nenhuma pista coletada.\n");
        return;
    }
    emOrdem(raiz);
    printf("--------------------------------------------------\n");
}

void inicializarHash() {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabelaHash[i] = NULL;
    }
}

unsigned int calcularHash(const char *nome) {
    unsigned int hashVal = 0;
    for (int i = 0; nome[i] != '\0'; i++) {
        hashVal = hashVal * 31 + nome[i]; 
    }
    return hashVal % TAMANHO_HASH;
}

Suspeito* buscarSuspeito(const char *nome) {
    unsigned int indice = calcularHash(nome);
    Suspeito *atual = tabelaHash[indice];

    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            return atual;
        }
        atual = atual->proximo;
    }
    return NULL;
}

void inserirHash(const char *suspeitoNome, const char *pistaTexto) {
    Suspeito *suspeito = buscarSuspeito(suspeitoNome);

    if (suspeito == NULL) {
        unsigned int indice = calcularHash(suspeitoNome);
        
        suspeito = (Suspeito*)malloc(sizeof(Suspeito));
        if (suspeito == NULL) return; 
        
        strncpy(suspeito->nome, suspeitoNome, MAX_SALA_NOME - 1);
        suspeito->nome[MAX_SALA_NOME - 1] = '\0';
        suspeito->contagemPistas = 0;
        suspeito->listaPistas = NULL;
        
        suspeito->proximo = tabelaHash[indice];
        tabelaHash[indice] = suspeito;
    }

    Associacao *novaAssociacao = (Associacao*)malloc(sizeof(Associacao));
    if (novaAssociacao == NULL) return;
    strncpy(novaAssociacao->pista, pistaTexto, MAX_PISTA_TEXTO - 1);
    novaAssociacao->pista[MAX_PISTA_TEXTO - 1] = '\0';
    novaAssociacao->proximo = suspeito->listaPistas;
    suspeito->listaPistas = novaAssociacao;
    
    suspeito->contagemPistas++;
}

void listarAssociacoes() {
    printf("--- Associacoes Suspeito x Pistas ---\n");
    for (int i = 0; i < TAMANHO_HASH; i++) {
        Suspeito *atual = tabelaHash[i];
        while (atual != NULL) {
            printf("\nSuspeito: %s (Pistas: %d)\n", atual->nome, atual->contagemPistas);
            Associacao *pistaAtual = atual->listaPistas;
            while (pistaAtual != NULL) {
                printf("  -> %s\n", pistaAtual->pista);
                pistaAtual = pistaAtual->proximo;
            }
            atual = atual->proximo;
        }
    }
    printf("-------------------------------------\n");
}

void encontrarSuspeitoMaisProvavel() {
    char maisProvavel[MAX_SALA_NOME] = "Nenhum";
    int maxPistas = -1;

    for (int i = 0; i < TAMANHO_HASH; i++) {
        Suspeito *atual = tabelaHash[i];
        while (atual != NULL) {
            if (atual->contagemPistas > maxPistas) {
                maxPistas = atual->contagemPistas;
                strncpy(maisProvavel, atual->nome, MAX_SALA_NOME - 1);
                maisProvavel[MAX_SALA_NOME - 1] = '\0';
            }
            atual = atual->proximo;
        }
    }

    if (maxPistas > 0) {
        printf("🕵️ SUSPEITO MAIS PROVAVEL: %s (com %d pistas associadas).\n", maisProvavel, maxPistas);
    } else {
        printf("Nao ha suspeitos com pistas suficientes.\n");
    }
}

void liberarHash() {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        Suspeito *suspeitoAtual = tabelaHash[i];
        while (suspeitoAtual != NULL) {
            Suspeito *tempSuspeito = suspeitoAtual;
            
            Associacao *pistaAtual = suspeitoAtual->listaPistas;
            while (pistaAtual != NULL) {
                Associacao *tempPista = pistaAtual;
                pistaAtual = pistaAtual->proximo;
                free(tempPista);
            }
            
            suspeitoAtual = suspeitoAtual->proximo;
            free(tempSuspeito);
        }
        tabelaHash[i] = NULL;
    }
}
