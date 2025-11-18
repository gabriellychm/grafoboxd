/*Gabrielly Christina Moreira e Marcos André Trajano da Silva Júnior

Nosso trabalho é baseado na plataforma de filmes Letterboxd, onde o grafo será utilizado para representar as conexões entre usuário e seus filmes. Cada algoritmo representará uma função da plataforma.

Algoritmos:
Busca em profundidade
Busca em largura
Dijkstra
Arvore geradora mínima - Prim
Componentes conexos
*/

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define MAX_FILMES 50
#define MAX_NOME 100

// structs
typedef struct no
{
    int v;
    struct no *prox;
    int peso;
} NO;
typedef NO *ptr_no;

typedef struct grafo
{
    ptr_no *adjacencia;
    int n;
} GRAFO;
typedef GRAFO *ptr_grafo;

typedef struct pilha
{
    int *elementos;
    int topo;
    int capacidade;
} PILHA;

typedef struct fila
{
    int *elementos;
    int inicio;
    int fim;
    int capacidade;
    int tamanho;
} FILA;

typedef struct item
{
    int prioridade;
    int vertice;
} ITEM;

typedef struct filaprioridade
{
    ITEM *v;
    int *indice;
    int n, tam;
} FILAPRIORIDADE;

typedef FILAPRIORIDADE *ptr_filaprioridade;

// funções da pilha para busca em profundidade

PILHA *criarPilha(int capacidade)
{
    PILHA *pilha = malloc(sizeof(PILHA));
    pilha->elementos = malloc(capacidade * sizeof(int));
    pilha->topo = -1;
    pilha->capacidade = capacidade;
    return pilha;
}
void empilhar(PILHA *pilha, int valor)
{
    if (pilha->topo < pilha->capacidade - 1)
        pilha->elementos[++pilha->topo] = valor;
}
int desempilhar(PILHA *pilha)
{
    if (pilha->topo >= 0)
        return pilha->elementos[pilha->topo--];
    return -1; // pilha vazia
}
int pilhaVazia(PILHA *pilha) { return pilha->topo == -1; }
void destruirPilha(PILHA *pilha)
{
    free(pilha->elementos);
    free(pilha);
}
// fim das funções da pilha

// funções da fila para busca em largura
FILA *criarFila(int capacidade)
{
    FILA *fila = malloc(sizeof(FILA));
    fila->elementos = malloc(capacidade * sizeof(int));
    fila->inicio = 0;
    fila->fim = -1;
    fila->capacidade = capacidade;
    fila->tamanho = 0;
    return fila;
}
void enfileirar(FILA *f, int v)
{
    if (f->tamanho == f->capacidade)
        return; // cheia
    f->fim = (f->fim + 1) % f->capacidade;
    f->elementos[f->fim] = v;
    f->tamanho++;
}
int desenfileirar(FILA *f)
{
    if (f->tamanho == 0)
        return -1; // vazia
    int val = f->elementos[f->inicio];
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->tamanho--;
    return val;
}

int filaVazia(FILA *f)
{
    return f->tamanho == 0;
}

void destruirFila(FILA *f)
{
    free(f->elementos);
    free(f);
}
// fim das funções da fila

// funções do grafo

ptr_grafo criarGrafo(int n)
{
    ptr_grafo grafo = malloc(sizeof(GRAFO));
    grafo->n = n;
    grafo->adjacencia = malloc(n * sizeof(ptr_no));
    for (int i = 0; i < n; i++)
        grafo->adjacencia[i] = NULL;
    return grafo;
}
void liberaLista(ptr_no lista)
{
    if (lista != NULL)
    {
        liberaLista(lista->prox);
        free(lista);
    }
}
void destroiGrafo(ptr_grafo grafo)
{
    if (!grafo)
        return;
    for (int i = 0; i < grafo->n; i++)
        liberaLista(grafo->adjacencia[i]);
    free(grafo->adjacencia);
    free(grafo);
}
void insereAresta(ptr_grafo grafo, int u, int v, int peso)
{
    if (!grafo)
        return;
    ptr_no novoElementoU = malloc(sizeof(NO));
    ptr_no novoElementoV = malloc(sizeof(NO));
    novoElementoU->v = v;
    novoElementoU->peso = peso;
    novoElementoU->prox = grafo->adjacencia[u];
    grafo->adjacencia[u] = novoElementoU;
    novoElementoV->v = u;
    novoElementoV->peso = peso;
    novoElementoV->prox = grafo->adjacencia[v];
    grafo->adjacencia[v] = novoElementoV;
}
void removeAresta(ptr_grafo grafo, int u, int v)
{
    if (!grafo)
        return;
    ptr_no atual = grafo->adjacencia[u], anterior = NULL;
    while (atual != NULL && atual->v != v)
    {
        anterior = atual;
        atual = atual->prox;
    }
    if (atual != NULL)
    {
        if (anterior == NULL)
            grafo->adjacencia[u] = atual->prox;
        else
            anterior->prox = atual->prox;
        free(atual);
    }
    atual = grafo->adjacencia[v];
    anterior = NULL;
    while (atual != NULL && atual->v != u)
    {
        anterior = atual;
        atual = atual->prox;
    }
    if (atual != NULL)
    {
        if (anterior == NULL)
            grafo->adjacencia[v] = atual->prox;
        else
            anterior->prox = atual->prox;
        free(atual);
    }
}

// peso da aresta u-v
int pesoAresta(ptr_grafo g, int u, int v)
{
    if (!g)
        return -1;
    for (ptr_no aux = g->adjacencia[u]; aux != NULL; aux = aux->prox)
        if (aux->v == v)
            return aux->peso;
    return -1;
}

// fila de prioridade para dijk e prim
ptr_filaprioridade criarFilaPrioridade(int tam)
{
    ptr_filaprioridade fila = malloc(sizeof(FILAPRIORIDADE));
    fila->v = malloc(tam * sizeof(ITEM));
    fila->indice = malloc(tam * sizeof(int));
    fila->n = tam;
    fila->tam = 0;
    for (int i = 0; i < tam; i++)
        fila->indice[i] = -1;
    return fila;
}
void insereFilaPrioridade(ptr_filaprioridade fila, int vertice, int prioridade)
{
    fila->v[fila->tam].vertice = vertice;
    fila->v[fila->tam].prioridade = prioridade;
    fila->indice[vertice] = fila->tam;
    fila->tam++;
}
int prioridadeValor(ptr_filaprioridade fila, int vertice)
{
    int i = fila->indice[vertice];
    return (i != -1) ? fila->v[i].prioridade : INT_MAX;
}
void diminuirPrioridade(ptr_filaprioridade fila, int vertice, int nova)
{
    int i = fila->indice[vertice];
    if (i == -1)
        return;
    fila->v[i].prioridade = nova;
    while (i > 0 && fila->v[(i - 1) / 2].prioridade > fila->v[i].prioridade)
    {
        ITEM tmp = fila->v[i];
        fila->v[i] = fila->v[(i - 1) / 2];
        fila->v[(i - 1) / 2] = tmp;
        fila->indice[fila->v[i].vertice] = i;
        fila->indice[fila->v[(i - 1) / 2].vertice] = (i - 1) / 2;
        i = (i - 1) / 2;
    }
}
int filaPrioridadeVazia(ptr_filaprioridade fila) { return fila->tam == 0; }
int extrairMinimo(ptr_filaprioridade fila)
{
    if (filaPrioridadeVazia(fila))
        return -1;
    int verticeMinimo = fila->v[0].vertice;
    // marcar removido
    fila->indice[verticeMinimo] = -1;
    // mover último para raiz (se houver)
    if (fila->tam > 1)
    {
        fila->v[0] = fila->v[fila->tam - 1];
        fila->indice[fila->v[0].vertice] = 0;
    }
    fila->tam--;

    int i = 0;
    while (2 * i + 1 < fila->tam)
    {
        int menorFilho = 2 * i + 1;
        if (menorFilho + 1 < fila->tam && fila->v[menorFilho + 1].prioridade < fila->v[menorFilho].prioridade)
            menorFilho++;
        if (fila->v[i].prioridade <= fila->v[menorFilho].prioridade)
            break;
        ITEM tmp = fila->v[i];
        fila->v[i] = fila->v[menorFilho];
        fila->v[menorFilho] = tmp;
        fila->indice[fila->v[i].vertice] = i;
        fila->indice[fila->v[menorFilho].vertice] = menorFilho;
        i = menorFilho;
    }
    return verticeMinimo;
}

// algoritmos

// busca em profundidade
int *buscaProfundidade(ptr_grafo grafo, int s)
{
    PILHA *pilha = criarPilha(grafo->n);
    int *visitado = malloc(grafo->n * sizeof(int));
    int *pai = malloc(grafo->n * sizeof(int));
    for (int i = 0; i < grafo->n; i++)
    {
        visitado[i] = 0;
        pai[i] = -1;
    }
    empilhar(pilha, s);
    pai[s] = s;
    while (!pilhaVazia(pilha))
    {
        int v = desempilhar(pilha);
        if (!visitado[v])
        {
            visitado[v] = 1;
            for (ptr_no aux = grafo->adjacencia[v]; aux != NULL; aux = aux->prox)
                if (!visitado[aux->v])
                {
                    empilhar(pilha, aux->v);
                    pai[aux->v] = v;
                }
        }
    }
    destruirPilha(pilha);
    free(visitado);
    return pai;
}

// busca em largura
int *buscaLarguraFila(ptr_grafo grafo, int s)
{
    FILA *fila = criarFila(grafo->n);
    int *visitado = malloc(grafo->n * sizeof(int));
    int *pai = malloc(grafo->n * sizeof(int));
    for (int i = 0; i < grafo->n; i++)
    {
        visitado[i] = 0;
        pai[i] = -1;
    }
    enfileirar(fila, s);
    visitado[s] = 1;
    pai[s] = s;
    while (!filaVazia(fila))
    {
        int v = desenfileirar(fila);
        for (ptr_no aux = grafo->adjacencia[v]; aux != NULL; aux = aux->prox)
            if (!visitado[aux->v])
            {
                visitado[aux->v] = 1;
                pai[aux->v] = v;
                enfileirar(fila, aux->v);
            }
    }
    destruirFila(fila);
    free(visitado);
    return pai;
}

// djikstra
int *dijkstra(ptr_grafo grafo, int s)
{
    int *pai = malloc(grafo->n * sizeof(int));
    int *dist = malloc(grafo->n * sizeof(int));

    for (int i = 0; i < grafo->n; i++)
    {
        pai[i] = -1;
        dist[i] = INT_MAX;
    }

    ptr_filaprioridade fila = criarFilaPrioridade(grafo->n);

    for (int i = 0; i < grafo->n; i++)
        insereFilaPrioridade(fila, i, dist[i]);

    dist[s] = 0;
    pai[s] = s;
    diminuirPrioridade(fila, s, 0);

    while (!filaPrioridadeVazia(fila))
    {
        int v = extrairMinimo(fila);
        if (v == -1)
            break;

        for (ptr_no aux = grafo->adjacencia[v]; aux != NULL; aux = aux->prox)
        {
            int nova = dist[v] + aux->peso;

            if (nova < dist[aux->v])
            {
                dist[aux->v] = nova;
                pai[aux->v] = v;
                diminuirPrioridade(fila, aux->v, nova);
            }
        }
    }

    free(fila->v);
    free(fila->indice);
    free(fila);

    return pai;
}

// prim
int *prim(ptr_grafo grafo, int s)
{
    int *pai = malloc(grafo->n * sizeof(int));
    int *chave = malloc(grafo->n * sizeof(int));
    int *incluso = malloc(grafo->n * sizeof(int));
    for (int i = 0; i < grafo->n; i++)
    {
        chave[i] = INT_MAX;
        pai[i] = -1;
        incluso[i] = 0;
    }
    chave[s] = 0;
    pai[s] = s;
    ptr_filaprioridade fila = criarFilaPrioridade(grafo->n);
    for (int i = 0; i < grafo->n; i++)
        insereFilaPrioridade(fila, i, chave[i]);
    while (!filaPrioridadeVazia(fila))
    {
        int u = extrairMinimo(fila);
        if (u == -1)
            break;
        incluso[u] = 1;
        for (ptr_no aux = grafo->adjacencia[u]; aux != NULL; aux = aux->prox)
        {
            int v = aux->v;
            int peso = aux->peso;
            if (!incluso[v] && peso < chave[v])
            {
                chave[v] = peso;
                pai[v] = u;
                diminuirPrioridade(fila, v, chave[v]);
            }
        }
    }
    free(chave);
    free(incluso);
    // liberar fila
    free(fila->v);
    free(fila->indice);
    free(fila);
    return pai;
}

// componentes conexos
void dfsComponentes(ptr_grafo grafo, int v, int comp, int *comps, int *visit)
{
    visit[v] = 1;
    comps[v] = comp;
    for (ptr_no aux = grafo->adjacencia[v]; aux != NULL; aux = aux->prox)
        if (!visit[aux->v])
            dfsComponentes(grafo, aux->v, comp, comps, visit);
}
int *componentesConexos(ptr_grafo grafo)
{
    int *visit = malloc(grafo->n * sizeof(int));
    int *comps = malloc(grafo->n * sizeof(int));
    for (int i = 0; i < grafo->n; i++)
    {
        visit[i] = 0;
        comps[i] = -1;
    }
    int comp = 0;
    for (int i = 0; i < grafo->n; i++)
        if (!visit[i])
            dfsComponentes(grafo, i, comp++, comps, visit);
    free(visit);
    return comps;
}

// funções para interação com nomes de filmes ao invés de índices
int buscarIndiceFilme(char filmes[][MAX_NOME], int n, char nome[])
{
    for (int i = 0; i < n; i++)
        if (strcmp(filmes[i], nome) == 0)
            return i;
    return -1;
}

void imprimirCaminhoPorNomes(int destino, int *pai, char filmes[][MAX_NOME])
{
    if (destino < 0)
        return;
    if (pai[destino] == -1)
    {
        printf("(sem caminho)\n");
        return;
    }

    int stack_size = 0;
    int *stack = malloc((1000) * sizeof(int));
    int cur = destino;
    while (1)
    {
        stack[stack_size++] = cur;
        if (pai[cur] == cur)
            break;
        cur = pai[cur];
        if (cur == -1)
            break;
    }
    // imprimir em ordem
    for (int i = stack_size - 1; i >= 0; i--)
    {
        printf("%s", filmes[stack[i]]);
        if (i != 0)
            printf(" -> ");
    }
    printf("\n");
    free(stack);
}

// calcula distância usando pesos da aresta
int calcularDistanciaPorPai(ptr_grafo grafo, int origem, int destino, int *pai)
{
    if (pai[destino] == -1)
        return INT_MAX;
    if (destino == origem)
        return 0;
    int total = 0;
    int atual = destino;
    while (atual != origem && atual != -1)
    {
        int p = pai[atual];
        if (p == -1)
            return INT_MAX;
        int w = pesoAresta(grafo, p, atual);
        if (w == -1)
            return INT_MAX;
        total += w;
        atual = p;
    }
    if (atual != origem)
        return INT_MAX;
    return total;
}

// menuuuuuuu
int main()
{
    setlocale(LC_ALL, "pt_BR.UTF-8");
    int menu, n = 0;
    ptr_grafo grafo = NULL;
    char filmes[MAX_FILMES][MAX_NOME];

    // inicializar nomes vazios
    for (int i = 0; i < MAX_FILMES; i++)
        filmes[i][0] = '\0';

    printf("=== GRAFOBOXD ===\n");

    while (1)
    {
        printf("\n1. Criar grafo e cadastrar filmes\n");
        printf("2. Conectar Filmes Semelhantes(Inserir Aresta)\n");
        printf("3. Desfazer Conexao Entre Filmes (Remover Aresta)\n");
        printf("4. Recomendacoes de Menor Caminho (BFS)\n");
        printf("5. Cadeias de Filmes (DFS)\n");
        printf("6. Filmes Mais Semelhantes com Peso (Dijkstra)\n");
        printf("7. Grupo de Filmes Relacionados (Componentes Conexos)\n");
        printf("8. Rede Minima de Todos os Filmes (Prim)\n");
        printf("9. Listar filmes cadastrados\n");
        printf("10. Imprimir grafo (lista de adjacencia)\n");
        printf("11. Destruir grafo\n");
        printf("12. Sair\n");
        printf("Escolha a opcao: ");

        if (scanf("%d", &menu) != 1)
        {
            // limpar buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
            printf("Entrada invalida.\n");
            continue;
        }

        if (menu == 12)
        {
            if (grafo)
                destroiGrafo(grafo);
            printf("Saindo...\n");
            break;
        }

        switch (menu)
        {
        case 1:
        {
            if (grafo)
            {
                printf("Ja existe um grafo criado. Deseja destruir e criar outro? (1-sim / 0-nao): ");
                int resp;
                if (scanf("%d", &resp) != 1)
                {
                    printf("Entrada invalida.\n");
                    break;
                }
                if (resp)
                {
                    destroiGrafo(grafo);
                    grafo = NULL;
                    n = 0;
                }
                else
                    break;
            }
            printf("Quantos filmes deseja cadastrar (1-%d)? ", MAX_FILMES);
            if (scanf("%d", &n) != 1 || n <= 0 || n > MAX_FILMES)
            {
                printf("Numero invalido.\n");
                n = 0;
                break;
            }
            // consumir newline
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
            for (int i = 0; i < n; i++)
            {
                printf("Filme %d: ", i);
                fgets(filmes[i], MAX_NOME, stdin);
                // remover newline final
                size_t L = strlen(filmes[i]);
                if (L > 0 && filmes[i][L - 1] == '\n')
                    filmes[i][L - 1] = '\0';
            }
            grafo = criarGrafo(n);
            printf("Grafo criado com %d filmes.\n", n);
            break;
        }

        case 2:
        {
            if (!grafo)
            {
                printf("Crie o grafo primeiro (opcao 1).\n");
                break;
            }
            // ler nomes com espaços
            char a[MAX_NOME], b[MAX_NOME];
            int peso;
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            } // limpar buffer
            printf("Nome do primeiro filme: ");
            fgets(a, MAX_NOME, stdin);
            if ((c = strlen(a)) > 0 && a[c - 1] == '\n')
                a[c - 1] = 0;
            printf("Nome do segundo filme: ");
            fgets(b, MAX_NOME, stdin);
            if ((c = strlen(b)) > 0 && b[c - 1] == '\n')
                b[c - 1] = 0;
            printf("Peso da conexao (inteiro positivo): ");
            if (scanf("%d", &peso) != 1)
            {
                printf("Peso invalido.\n");
                break;
            }
            int u = buscarIndiceFilme(filmes, n, a), v = buscarIndiceFilme(filmes, n, b);
            if (u == -1 || v == -1)
            {
                printf("Um dos filmes nao foi encontrado.\n");
                break;
            }
            insereAresta(grafo, u, v, peso);
            printf("Conexao criada: %s --(%d)-- %s\n", a, peso, b);
            break;
        }

        case 3:
        {
            if (!grafo)
            {
                printf("Crie o grafo primeiro (opcao 1).\n");
                break;
            }
            char a[MAX_NOME], b[MAX_NOME];
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
            printf("Nome do primeiro filme: ");
            fgets(a, MAX_NOME, stdin);
            if ((c = strlen(a)) > 0 && a[c - 1] == '\n')
                a[c - 1] = 0;
            printf("Nome do segundo filme: ");
            fgets(b, MAX_NOME, stdin);
            if ((c = strlen(b)) > 0 && b[c - 1] == '\n')
                b[c - 1] = 0;
            int u = buscarIndiceFilme(filmes, n, a), v = buscarIndiceFilme(filmes, n, b);
            if (u == -1 || v == -1)
            {
                printf("Um dos filmes nao foi encontrado.\n");
                break;
            }
            removeAresta(grafo, u, v);
            printf("Conexao removida entre %s e %s (se existia).\n", a, b);
            break;
        }

        case 4:
        { // BFS (marcos depois tenta arrumar essa parte!!!)
            if (!grafo)
            {
                printf("Crie o grafo primeiro (opcao 1).\n");
                break;
            }
            char inicio[MAX_NOME], destino[MAX_NOME];
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
            printf("Filme origem (BFS): ");
            fgets(inicio, MAX_NOME, stdin);
            if ((c = strlen(inicio)) > 0 && inicio[c - 1] == '\n')
                inicio[c - 1] = 0;
            int s = buscarIndiceFilme(filmes, n, inicio);
            if (s == -1)
            {
                printf("Filme nao encontrado.\n");
                break;
            }
            int *pai = buscaLarguraFila(grafo, s);
            printf("Deseja ver caminho da origem para qual filme? (digite nome ou apenas ENTER para pular)\n");
            printf("Destino: ");
            fgets(destino, MAX_NOME, stdin);
            if ((c = strlen(destino)) > 0 && destino[c - 1] == '\n')
                destino[c - 1] = 0;
            if (strlen(destino) == 0)
            {
                printf("Pais (por indice):\n");
                for (int i = 0; i < grafo->n; i++)
                    printf("%s : %d\n", filmes[i], pai[i]);
            }
            else
            {
                int d = buscarIndiceFilme(filmes, n, destino);
                if (d == -1)
                {
                    printf("Destino nao encontrado.\n");
                    free(pai);
                    break;
                }
                if (pai[d] == -1)
                    printf("Nao existe caminho de %s para %s\n", inicio, destino);
                else
                {
                    printf("Caminho: ");
                    imprimirCaminhoPorNomes(d, pai, filmes);
                }
            }
            free(pai);
            break;
        }

        case 5:
        { // DFS
            if (!grafo)
            {
                printf("Crie o grafo primeiro (opcao 1).\n");
                break;
            }
            char inicio[MAX_NOME], destino[MAX_NOME];
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
            printf("Filme origem (DFS): ");
            fgets(inicio, MAX_NOME, stdin);
            if ((c = strlen(inicio)) > 0 && inicio[c - 1] == '\n')
                inicio[c - 1] = 0;
            int s = buscarIndiceFilme(filmes, n, inicio);
            if (s == -1)
            {
                printf("Filme nao encontrado.\n");
                break;
            }
            int *pai = buscaProfundidade(grafo, s);
            printf("Deseja ver caminho da origem para qual filme? (digite nome ou apenas ENTER para pular)\n");
            printf("Destino: ");
            fgets(destino, MAX_NOME, stdin);
            if ((c = strlen(destino)) > 0 && destino[c - 1] == '\n')
                destino[c - 1] = 0;
            if (strlen(destino) == 0)
            {
                printf("Pais (por indice):\n");
                for (int i = 0; i < grafo->n; i++)
                    printf("%s : %d\n", filmes[i], pai[i]);
            }
            else
            {
                int d = buscarIndiceFilme(filmes, n, destino);
                if (d == -1)
                {
                    printf("Destino nao encontrado.\n");
                    free(pai);
                    break;
                }
                if (pai[d] == -1)
                    printf("Nao existe caminho de %s para %s\n", inicio, destino);
                else
                {
                    printf("Caminho: ");
                    imprimirCaminhoPorNomes(d, pai, filmes);
                }
            }
            free(pai);
            break;
        }

        case 6:
        { // dijkstra (complicado...)
            if (!grafo)
            {
                printf("Crie o grafo primeiro (opcao 1).\n");
                break;
            }
            char inicio[MAX_NOME], destino[MAX_NOME];
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
            printf("Filme origem (Dijkstra): ");
            fgets(inicio, MAX_NOME, stdin);
            if ((c = strlen(inicio)) > 0 && inicio[c - 1] == '\n')
                inicio[c - 1] = 0;
            int s = buscarIndiceFilme(filmes, n, inicio);
            if (s == -1)
            {
                printf("Filme nao encontrado.\n");
                break;
            }
            int *pai = dijkstra(grafo, s);
            printf("Destino (digite nome) ou ENTER para ver todas as distancias: ");
            fgets(destino, MAX_NOME, stdin);
            if ((c = strlen(destino)) > 0 && destino[c - 1] == '\n')
                destino[c - 1] = 0;
            if (strlen(destino) == 0)
            {
                printf("Distancias aproximadas a partir de %s:\n", inicio);
                for (int i = 0; i < grafo->n; i++)
                {
                    int dist = calcularDistanciaPorPai(grafo, s, i, pai);
                    if (dist == INT_MAX)
                        printf("%s : INF\n", filmes[i]);
                    else
                        printf("%s : %d\n", filmes[i], dist);
                }
            }
            else
            {
                int d = buscarIndiceFilme(filmes, n, destino);
                if (d == -1)
                {
                    printf("Destino nao encontrado.\n");
                    free(pai);
                    break;
                }
                if (pai[d] == -1)
                {
                    printf("Nao existe caminho de %s para %s\n", inicio, destino);
                    free(pai);
                    break;
                }
                int dist = calcularDistanciaPorPai(grafo, s, d, pai);
                printf("Caminho de custo %d: ", dist);
                imprimirCaminhoPorNomes(d, pai, filmes);
            }
            free(pai);
            break;
        }

        case 7:
        { // Componentes conexos
            if (!grafo)
            {
                printf("Crie o grafo primeiro (opcao 1).\n");
                break;
            }
            int *comps = componentesConexos(grafo);
            // contar quantos componentes existem
            int maxcomp = -1;
            for (int i = 0; i < grafo->n; i++)
                if (comps[i] > maxcomp)
                    maxcomp = comps[i];
            int qtd = maxcomp + 1;
            printf("Foram encontrados %d componente(s):\n", qtd);
            for (int c = 0; c < qtd; c++)
            {
                printf("Componente %d: ", c);
                int first = 1;
                for (int i = 0; i < grafo->n; i++)
                    if (comps[i] == c)
                    {
                        if (!first)
                            printf(", ");
                        printf("%s", filmes[i]);
                        first = 0;
                    }
                printf("\n");
            }
            free(comps);
            break;
        }

        case 8:
        { // Prim
            if (!grafo)
            {
                printf("Crie o grafo primeiro (opcao 1).\n");
                break;
            }
            char inicio[MAX_NOME];
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
            {
            }
            printf("Vertice inicial para Prim (nome do filme): ");
            fgets(inicio, MAX_NOME, stdin);
            if ((c = strlen(inicio)) > 0 && inicio[c - 1] == '\n')
                inicio[c - 1] = 0;
            int s = buscarIndiceFilme(filmes, n, inicio);
            if (s == -1)
            {
                printf("Filme nao encontrado.\n");
                break;
            }
            int *pai = prim(grafo, s);
            // imprimir arestas de primm
            int total = 0;
            printf("Arvore geradora minima (pai -> filho) e pesos:\n");
            for (int i = 0; i < grafo->n; i++)
            {
                if (pai[i] == -1 || pai[i] == i)
                    continue;
                int w = pesoAresta(grafo, pai[i], i);
                if (w == -1)
                    w = 0;
                total += w;
                printf("%s --(%d)-- %s\n", filmes[pai[i]], w, filmes[i]);
            }
            printf("Custo total (soma das arestas escolhidas): %d\n", total);
            free(pai);
            break;
        }

        case 9:
        { // listar filmes
            if (n == 0)
            {
                printf("Nenhum filme cadastrado.\n");
                break;
            }
            printf("Filmes cadastrados:\n");
            for (int i = 0; i < n; i++)
                printf("%d: %s\n", i, filmes[i]);
            break;
        }

        case 10:
        { // imprimir grafo (lista de adjacencia)
            if (!grafo)
            {
                printf("Crie o grafo primeiro (opcao 1).\n");
                break;
            }
            printf("Lista de adjacencia:\n");
            for (int i = 0; i < grafo->n; i++)
            {
                printf("%s: ", filmes[i]);
                for (ptr_no aux = grafo->adjacencia[i]; aux != NULL; aux = aux->prox)
                {
                    printf("(%s,%d) ", filmes[aux->v], aux->peso);
                }
                printf("\n");
            }
            break;
        }

        case 11:
        { // destruir grafo
            if (!grafo)
            {
                printf("Nao existe grafo criado.\n");
                break;
            }
            destroiGrafo(grafo);
            grafo = NULL;
            n = 0;
            for (int i = 0; i < MAX_FILMES; i++)
                filmes[i][0] = '\0';
            printf("Grafo destruido.\n");
            break;
        }

        default:
            printf("Opcao invalida.\n");
            break;
        }
    }

    return 0;
}
