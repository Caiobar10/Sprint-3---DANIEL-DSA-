/*
 * blua.c — Gerenciamento de fila de teleconsulta | Care Plus
 *
 * Fila  (FIFO) : lista encadeada simples, dois ponteiros (inicio/fim)
 * Pilha (LIFO) : lista encadeada simples, ponteiro de topo com limite N
 * Sem uso de bibliotecas de estruturas prontas.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Tipos ------------------------------------------------------------ */

typedef struct NoPaciente {
    int  id;
    char nome[100];
    char queixa[200];
    struct NoPaciente *proximo;
} NoPaciente;

typedef struct NoHistorico {
    int  id;
    char nome[100];
    char medico[100];
    char queixa[200];
    struct NoHistorico *anterior;
} NoHistorico;

typedef struct {
    NoPaciente *inicio;
    NoPaciente *fim;
    int tamanho;
} Fila;

typedef struct {
    NoHistorico *topo;
    int tamanho;
    int capacidade_maxima;
} Pilha;

static int proximo_id = 1;

/* --- Fila ------------------------------------------------------------- */

void inicializar_fila(Fila *f) {
    f->inicio = f->fim = NULL;
    f->tamanho = 0;
}

int fila_vazia(Fila *f) {
    return f->inicio == NULL;
}

NoPaciente *peek_fila(Fila *f) {
    return f->inicio;
}

void enqueue(Fila *f, const char *nome, const char *queixa) {
    NoPaciente *novo = malloc(sizeof(NoPaciente));
    if (!novo) { fprintf(stderr, "[ERRO] malloc falhou\n"); return; }

    novo->id = proximo_id++;
    strncpy(novo->nome,   nome,   99);
    strncpy(novo->queixa, queixa, 199);
    novo->proximo = NULL;

    if (fila_vazia(f))
        f->inicio = f->fim = novo;
    else {
        f->fim->proximo = novo;
        f->fim = novo;
    }
    f->tamanho++;
    printf("[FILA] %s enfileirado(a) (ID: %d)\n", novo->nome, novo->id);
}

/* Retorna cópia por valor; id == -1 sinaliza fila vazia ao chamador. */
NoPaciente dequeue(Fila *f) {
    NoPaciente vazio = {0};
    if (fila_vazia(f)) {
        printf("[AVISO] Fila vazia.\n");
        vazio.id = -1;
        return vazio;
    }

    NoPaciente *saindo = f->inicio;
    NoPaciente resultado = *saindo;
    resultado.proximo = NULL;

    f->inicio = saindo->proximo;
    if (!f->inicio) f->fim = NULL;

    free(saindo);
    f->tamanho--;
    return resultado;
}

/* --- Pilha ------------------------------------------------------------ */

void inicializar_pilha(Pilha *p, int capacidade) {
    p->topo = NULL;
    p->tamanho = 0;
    p->capacidade_maxima = capacidade;
}

int pilha_vazia(Pilha *p) {
    return p->topo == NULL;
}

NoHistorico *peek_pilha(Pilha *p) {
    return p->topo;
}

/*
 * Ao atingir capacidade_maxima, descarta o nó do fundo (O(n)) para
 * preservar O(1) em todas as operações do caminho crítico.
 */
void push(Pilha *p, int id, const char *nome, const char *medico, const char *queixa) {
    if (p->tamanho >= p->capacidade_maxima) {
        if (p->tamanho == 1) {
            free(p->topo);
            p->topo = NULL;
            p->tamanho = 0;
        } else {
            NoHistorico *cursor = p->topo;
            while (cursor->anterior && cursor->anterior->anterior)
                cursor = cursor->anterior;
            free(cursor->anterior);
            cursor->anterior = NULL;
            p->tamanho--;
        }
    }

    NoHistorico *novo = malloc(sizeof(NoHistorico));
    if (!novo) { fprintf(stderr, "[ERRO] malloc falhou\n"); return; }

    novo->id = id;
    strncpy(novo->nome,   nome,   99);
    strncpy(novo->medico, medico, 99);
    strncpy(novo->queixa, queixa, 199);
    novo->anterior = p->topo;
    p->topo = novo;
    p->tamanho++;

    printf("[HISTORICO] %s — Dr(a). %s\n", novo->nome, novo->medico);
}

/* Retorna cópia por valor; id == -1 sinaliza pilha vazia ao chamador. */
NoHistorico pop(Pilha *p) {
    NoHistorico vazio = {0};
    if (pilha_vazia(p)) {
        printf("[AVISO] Historico vazio.\n");
        vazio.id = -1;
        return vazio;
    }

    NoHistorico *saindo = p->topo;
    NoHistorico resultado = *saindo;
    resultado.anterior = NULL;

    p->topo = saindo->anterior;
    free(saindo);
    p->tamanho--;
    return resultado;
}

/* --- Exibição --------------------------------------------------------- */

void exibir_fila(Fila *f, const char *titulo) {
    printf("\n--- %s (%d) ---\n", titulo, f->tamanho);
    if (fila_vazia(f)) { printf("  (vazia)\n"); return; }
    NoPaciente *n = f->inicio;
    for (int i = 1; n; i++, n = n->proximo)
        printf("  %d. [%d] %s — \"%s\"\n", i, n->id, n->nome, n->queixa);
}

void exibir_historico(Pilha *p) {
    printf("\n--- Historico (topo = mais recente) (%d) ---\n", p->tamanho);
    if (pilha_vazia(p)) { printf("  (vazio)\n"); return; }
    NoHistorico *n = p->topo;
    for (int i = 1; n; i++, n = n->anterior)
        printf("  %d. [%d] %s | Dr(a). %s | \"%s\"\n",
               i, n->id, n->nome, n->medico, n->queixa);
}

static void separador(const char *titulo) {
    printf("\n========================================\n");
    printf("  %s\n", titulo);
    printf("========================================\n");
}

/* --- Cenários de demonstração ---------------------------------------- */

void cenario_1(void) {
    separador("Cenario 1 — Fluxo normal de atendimento");

    Fila fila; Pilha historico;
    inicializar_fila(&fila);
    inicializar_pilha(&historico, 5);

    enqueue(&fila, "Ana Lima",    "Dor de cabeca persistente");
    enqueue(&fila, "Bruno Silva", "Febre ha 3 dias");
    enqueue(&fila, "Carla Melo",  "Tosse seca");
    exibir_fila(&fila, "Fila de espera");

    printf("\n[Dr. Rodrigues atendendo...]\n");
    while (!fila_vazia(&fila)) {
        NoPaciente p = dequeue(&fila);
        push(&historico, p.id, p.nome, "Rodrigues", p.queixa);
    }
    exibir_historico(&historico);

    while (!pilha_vazia(&historico)) pop(&historico);
}

void cenario_2(void) {
    separador("Cenario 2 — Emergencia com fila prioritaria");

    Fila normal, emergencia; Pilha historico;
    inicializar_fila(&normal);
    inicializar_fila(&emergencia);
    inicializar_pilha(&historico, 5);

    enqueue(&normal, "Diego Faria",   "Dor no joelho");
    enqueue(&normal, "Eva Costa",     "Consulta de rotina");
    enqueue(&normal, "Fabio Nunes",   "Pressao alta leve");

    printf("\n[*** ALERTA CRITICO ***]\n");
    enqueue(&emergencia, "Gisele Ramos", "Dor no peito e falta de ar");

    exibir_fila(&emergencia, "EMERGENCIA (prioridade)");
    exibir_fila(&normal,     "Fila normal (aguarda)");

    /* Emergências esgotadas antes de retomar a fila normal */
    printf("\n[Dra. Camila atendendo...]\n");
    while (!fila_vazia(&emergencia)) {
        NoPaciente p = dequeue(&emergencia);
        printf("[URGENTE] %s\n", p.nome);
        push(&historico, p.id, p.nome, "Camila", p.queixa);
    }
    while (!fila_vazia(&normal)) {
        NoPaciente p = dequeue(&normal);
        push(&historico, p.id, p.nome, "Camila", p.queixa);
    }
    exibir_historico(&historico);

    while (!pilha_vazia(&historico)) pop(&historico);
}

void cenario_3(void) {
    separador("Cenario 3 — Cancelamento e auditoria");

    Fila fila; Pilha historico;
    inicializar_fila(&fila);
    inicializar_pilha(&historico, 3); /* politica: audita ultimas 3 consultas */

    enqueue(&fila, "Helio Dias",   "Insonia cronica");
    enqueue(&fila, "Iris Borges",  "Ansiedade");
    enqueue(&fila, "Jonas Lima",   "Check-up geral");
    enqueue(&fila, "Karina Souza", "Dor lombar");
    enqueue(&fila, "Lucas Teles",  "Alergia respiratoria");

    printf("\n[Dr. Marcos atendendo...]\n");
    while (!fila_vazia(&fila)) {
        NoPaciente p = dequeue(&fila);
        push(&historico, p.id, p.nome, "Marcos", p.queixa);
    }
    exibir_historico(&historico);

    printf("\n[Admin: desfazendo ultimo registro — erro de reatribuicao]\n");
    NoHistorico cancelado = pop(&historico);
    if (cancelado.id != -1)
        printf("[DESFEITO] %s (ID: %d)\n", cancelado.nome, cancelado.id);

    exibir_historico(&historico);

    NoHistorico *topo = peek_pilha(&historico);
    if (topo) printf("\n[AUDITORIA peek] Topo atual: %s\n", topo->nome);

    while (!pilha_vazia(&historico)) pop(&historico);
}

/* --- Estado global do menu ------------------------------------------- */

static Fila  g_fila_normal;
static Fila  g_fila_emergencia;
static Pilha g_historico;

static void menu_enfileirar(int emergencia) {
    char nome[100], queixa[200];
    printf("Nome: ");    scanf(" %99[^\n]",  nome);
    printf("Queixa: "); scanf(" %199[^\n]", queixa);
    enqueue(emergencia ? &g_fila_emergencia : &g_fila_normal, nome, queixa);
}

static void menu_atender(void) {
    char medico[100];
    printf("Medico: "); scanf(" %99[^\n]", medico);

    Fila *origem = !fila_vazia(&g_fila_emergencia)
                   ? &g_fila_emergencia
                   : &g_fila_normal;

    if (fila_vazia(origem)) { printf("[AVISO] Sem pacientes.\n"); return; }

    NoPaciente p = dequeue(origem);
    push(&g_historico, p.id, p.nome, medico, p.queixa);
}

/* ---- main ------------------------------------------------------------ */

int main(void) {
    inicializar_fila(&g_fila_normal);
    inicializar_fila(&g_fila_emergencia);
    inicializar_pilha(&g_historico, 10);

    printf("============================================\n");
    printf("  BLUA — Fila de Teleconsulta | Care Plus  \n");
    printf("============================================\n");

    int op;
    do {
        printf("\n[1] Enfileirar  [2] Emergencia  [3] Atender\n");
        printf("[4] Cancelar    [5] Ver estado  [6] Peek\n");
        printf("[7] Cenario 1   [8] Cenario 2   [9] Cenario 3\n");
        printf("[0] Sair\nOpcao: ");
        scanf("%d", &op);

        switch (op) {
            case 1: menu_enfileirar(0); break;
            case 2: menu_enfileirar(1); break;
            case 3: menu_atender();     break;
            case 4: {
                NoHistorico c = pop(&g_historico);
                if (c.id != -1)
                    printf("[CANCELADO] %s (ID: %d)\n", c.nome, c.id);
                break;
            }
            case 5:
                exibir_fila(&g_fila_emergencia, "Emergencia");
                exibir_fila(&g_fila_normal,     "Normal");
                exibir_historico(&g_historico);
                break;
            case 6: {
                NoPaciente  *pe = peek_fila(&g_fila_emergencia);
                NoPaciente  *pn = peek_fila(&g_fila_normal);
                NoHistorico *ph = peek_pilha(&g_historico);
                printf("Emergencia : %s\n", pe ? pe->nome : "(vazia)");
                printf("Normal     : %s\n", pn ? pn->nome : "(vazia)");
                printf("Historico  : %s\n", ph ? ph->nome : "(vazio)");
                break;
            }
            case 7: cenario_1(); break;
            case 8: cenario_2(); break;
            case 9: cenario_3(); break;
            case 0: printf("Encerrando.\n"); break;
            default: printf("[ERRO] Opcao invalida.\n");
        }
    } while (op != 0);

    while (!fila_vazia(&g_fila_normal))     dequeue(&g_fila_normal);
    while (!fila_vazia(&g_fila_emergencia)) dequeue(&g_fila_emergencia);
    while (!pilha_vazia(&g_historico))      pop(&g_historico);

    return 0;
}
