#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>



//--------------------------------ESTRUTURAS----------------------------------------

//Estrutura do produto
typedef struct
{
    char nome;
    float preco;
} Produto;

//Lista de produtos de um supermercado
typedef struct listaProdutos
{
    Produto produto;
    struct listaProdutos *prox;
} ListaProdutos;

//Estrutura do supermercado
typedef struct
{
    char nome;
    ListaProdutos *produtos;
    float frete;
} Supermercado;

//Lista de supermercados
typedef struct listaSupermercados
{
    Supermercado supermercado;
    struct listaSupermercados *prox;
} ListaSupermercados;

//Lista de compra que contem o produto e o supermercado de onde comprou 
typedef struct listaProdSup
{
    Produto produto;
    char supermercado;
    struct listaProdSup *prox;
} ListaProdSup;

//Estrutura de um estado, que é uma lista de produtos comprados, com o custo atual (custo total das compras até aquele estado) e o custo da heuristica
typedef struct
{
    ListaProdSup *prodSup;
    float custo_atual;
    float custo_heuristico;
} Estado;

//Estrutura de um nó, que é o estado junto com o endereço do nó que o gerou
typedef struct no
{
    Estado estado;
    struct no *paiNo;
    struct no *prox;
} No;

//Estrutura da pilha
struct pilha
{
    No *topo;
};
typedef struct pilha Pilha;



//--------------------------------ASSINATURAS DAS FUNÇÕES----------------------------------------

//->MANIPULAÇÃO DA PILHA:

//Cria e inicializa uma pilha
Pilha *CriarPilha();

//Adiciona um nó no inicio da pilha
void push(Pilha *p, Estado estado, No *paiNo);

//Funçaõ que remove da pilha um determinado nó. Ser´a usada para remover o nó com o menor custo total.
void remover_no(Pilha *p, No *no);

void imprimir_pilha(Pilha *p);

//->Execução do A*
//Encontra o preço de um dado produto em uma lista de produtos, retornando -1 caso não encontrado
float preco_produto(char produto, ListaProdutos *lista_produtos);

//A heurística escolhida foi a soma dos menores valores de cada produto, sem considerar o frete
float heuristica(char *produtos_escolhidos, ListaSupermercados *lista_supermercados);

//Adiciona os nós referentes ao proximo produto na pilha (Exemplo: [P1, P2...] [S1, S2...] -> adiciona na pilha os nós: P1S1, P1S2, ...)
//Esses nós são os filhos de noAtual, que é o nó com o menor custo total, portanto o nó que será aberto.
void adicionar_nos(Pilha *p, No *noAtual, ListaSupermercados *lista_supermercados, char *produtos_escolhidos);

//Encontra o nó com o menor custo na pilha e abre ele, adicionando seus filhos na pilha 
No* abrir_no_menor_custo(Pilha *p, ListaSupermercados *lista_supermercados, char *produtos_originais);

//Executa o algoritmo A* para encontrar a solução com o menor custo
void A_Estrela(Pilha *p, ListaSupermercados *lista_supermercados, char *produtos_originais);


//--------------------------------MAIN----------------------------------------

int main()
{
    setlocale(LC_ALL, "portuguese");

    ListaProdutos produtos_supermercado1[6] = 
    {
        {{'A', 3}, &produtos_supermercado1[1]},
        {{'B', 4}, &produtos_supermercado1[2]},
        {{'C', 2}, &produtos_supermercado1[3]},
        {{'D', 1}, &produtos_supermercado1[4]},
        {{'E', 3}, &produtos_supermercado1[5]},
        {{'F', 8}, NULL}
    };
    ListaSupermercados supermercado1 = {{'1', produtos_supermercado1, 2.0}, NULL};

    ListaProdutos produtos_supermercado2[6] = 
    {
        {{'A', 2}, &produtos_supermercado2[1]},
        {{'B', 1}, &produtos_supermercado2[2]},
        {{'C', 3.5}, &produtos_supermercado2[3]},
        {{'D', 4.5}, &produtos_supermercado2[4]},
        {{'E', 1}, &produtos_supermercado2[5]},
        {{'F', 5.5}, NULL}
    };
    ListaSupermercados supermercado2 = {{'2', produtos_supermercado2, 1.0}, NULL};

    ListaProdutos produtos_supermercado3[6] = 
    {
        {{'A', 4.5}, &produtos_supermercado3[1]},
        {{'B', 5}, &produtos_supermercado3[2]},
        {{'C', 1}, &produtos_supermercado3[3]},
        {{'D', 3}, &produtos_supermercado3[4]},
        {{'E', 4}, &produtos_supermercado3[5]},
        {{'F', 1}, NULL}
    };
    ListaSupermercados supermercado3 = {{'3', produtos_supermercado3, 0.5}, NULL};

    ListaSupermercados *lista_supermercados = &supermercado1;
    supermercado1.prox = &supermercado2;
    supermercado2.prox = &supermercado3;

    char produtos_escolhidos[4] = {'A', 'D', 'F', '\0'};

    Pilha *p = CriarPilha();

    A_Estrela(p, lista_supermercados, produtos_escolhidos);

    printf("\n\nObs:\n");
    printf("Custo total ao comprar apenas no supermercado 1: R$%.2f\n", 
            produtos_supermercado1[0].produto.preco+produtos_supermercado1[3].produto.preco+produtos_supermercado1[5].produto.preco + supermercado1.supermercado.frete);
    printf("Custo total ao comprar apenas no supermercado 2: R$%.2f\n", 
                produtos_supermercado2[0].produto.preco+produtos_supermercado2[3].produto.preco+produtos_supermercado2[5].produto.preco + supermercado2.supermercado.frete);
    printf("Custo total ao comprar apenas no supermercado 3: R$%.2f\n", 
                produtos_supermercado3[0].produto.preco+produtos_supermercado3[3].produto.preco+produtos_supermercado3[5].produto.preco + supermercado3.supermercado.frete);

    return 0;
}

//--------------------------------ASSINATURAS DAS FUNÇÕES----------------------------------------

//Cria e inicializa uma pilha
Pilha *CriarPilha()
{
    Pilha *p;
    p = (Pilha *)malloc(sizeof(Pilha));
    p->topo = NULL;
    return p;
}

//Adiciona um nó no inicio da pilha
void push(Pilha *p, Estado estado, No *paiNo)
{
    No *novoNo = (No *)malloc(sizeof(No));

    novoNo->estado = estado;
    novoNo->paiNo = paiNo;

    if (p->topo != NULL)
    {
        novoNo->prox = p->topo;
    }
    else
    {
        novoNo->prox = NULL;
    }

    p->topo = novoNo;
}

//Funçaõ que remove da pilha um determinado nó. Ser´a usada para remover o nó com o menor custo total.
void remover_no(Pilha *p, No *no)
{
    if (p->topo == NULL)
    {
        return;
    }

    if (p->topo == no)
    {
        p->topo = p->topo->prox;
        return;
    }

    No *atual = p->topo;
    while (atual->prox != NULL && atual->prox != no)
    {
        atual = atual->prox;
    }

    if (atual->prox == NULL)
    {
        return;
    }

    atual->prox = no->prox;
}

void imprimir_pilha(Pilha *p)
{
    No *atual = p->topo;
    while (atual != NULL)
    {
        printf("Produto: %c, Supermercado: %c, Custo Atual: %.2f, Custo Heurística: %.2f\n",
               atual->estado.prodSup->produto.nome,
               atual->estado.prodSup->supermercado,
               atual->estado.custo_atual,
               atual->estado.custo_heuristico);

        atual = atual->prox;
    }
}


//Encontra o preço de um dado produto em uma lista de produtos, retornando -1 caso não encontrado
float preco_produto(char produto, ListaProdutos *lista_produtos)
{
    while (lista_produtos != NULL)
    {
        if (lista_produtos->produto.nome == produto)
        {
            return lista_produtos->produto.preco;
        }
        lista_produtos = lista_produtos->prox;
    }
    return -1; 
}


//A heurística escolhida foi a soma dos menores valores de cada produto, sem considerar o frete
float heuristica(char *produtos_escolhidos, ListaSupermercados *lista_supermercados) 
{
    float custo_heuristico = 0;

    //Para cada produto na lista, percorre todos os supermercados para encontrar o menor preço dele e soma o preço na heuristica
    for (int i = 0; produtos_escolhidos[i] != '\0'; i++) 
    {
        char produto = produtos_escolhidos[i];
        float menor_preco = -1; 

        ListaSupermercados *temp_supermercados = lista_supermercados;
        while (temp_supermercados != NULL) 
        {
            float preco = preco_produto(produto, temp_supermercados->supermercado.produtos);
            if (preco >= 0 && (preco < menor_preco || menor_preco < 0)) 
            {
                menor_preco = preco;
            }
            temp_supermercados = temp_supermercados->prox;
        }

        if (menor_preco < 0) 
        {
            //Caso o produto não exista em nunhum dos supermercados
            return -1;
        }

        custo_heuristico += menor_preco;
    }

    return custo_heuristico;
}


//Adiciona os nós referentes ao proximo produto na pilha (Exemplo: [P1, P2...] [S1, S2...] -> adiciona na pilha os nós: P1S1, P1S2, ...)
//Esses nós são os filhos de noAtual, que é o nó com o menor custo total, portanto o nó que será aberto.
void adicionar_nos(Pilha *p, No *noAtual, ListaSupermercados *lista_supermercados, char *produtos_escolhidos)
{
    //Percorre todos os supermrcados, adicionando na pilha um nó para cada um deles
    for (ListaSupermercados *temp_supermercados = lista_supermercados; temp_supermercados != NULL; temp_supermercados = temp_supermercados->prox)
    {
        Supermercado *supermercado_atual = &temp_supermercados->supermercado;

        float preco = preco_produto(produtos_escolhidos[0], supermercado_atual->produtos);

        //Verificação se tem algum ancestral que foi selecionado no supermercado da iteração atual, para que não seja contabilizado o frete mais de uma vez
        int adicionaFrete = 1;
        No *aux = noAtual;
        while (aux != NULL)
        {
            char supermercadoPai = aux->estado.prodSup->supermercado;
            if (supermercado_atual->nome == supermercadoPai)
            {
                adicionaFrete = 0;
            }

            aux = aux->paiNo;
        }

        //Cria uma lista de compra para o produto no supermercado atual
        ListaProdSup *nova_lista = (ListaProdSup *)malloc(sizeof(ListaProdSup));
        nova_lista->produto.nome = produtos_escolhidos[0];
        nova_lista->produto.preco = preco;
        nova_lista->supermercado = supermercado_atual->nome;
        nova_lista->prox = NULL;

        //Cria um novo estado com a compra
        Estado estado_filho;
        estado_filho.prodSup = nova_lista;
        //Se não for a raiz, o custo atual do filho é o custo real do pai mais o preço do produto atual
        if (noAtual != NULL)
        {
            estado_filho.custo_atual = noAtual->estado.custo_atual + preco;
        }
        else
        {
            estado_filho.custo_atual = preco;
        }

        if (adicionaFrete == 1)
        {
            estado_filho.custo_atual += supermercado_atual->frete;
        }

        //Cria uma cópia dos produtos escolhidos pelos ancestrais e remove o produto atual
        char produtos_copia_filho[strlen(produtos_escolhidos) + 1];
        strcpy(produtos_copia_filho, produtos_escolhidos);
        char *ptr_filho = strchr(produtos_copia_filho, produtos_escolhidos[0]);
        if (ptr_filho != NULL)
        {
            memmove(ptr_filho, ptr_filho + 1, strlen(ptr_filho));
        }

        //Calcula a heuristica dos produtos que ainda faltam ser escolhidos
        estado_filho.custo_heuristico = heuristica(produtos_copia_filho, lista_supermercados);

        //Adiciona o estado na pilha
        push(p, estado_filho, noAtual);
    }
}



//Encontra o nó com o menor custo na pilha e abre ele, adicionando seus filhos na pilha 
No* abrir_no_menor_custo(Pilha *p, ListaSupermercados *lista_supermercados, char *produtos_originais)
{
    if (p->topo == NULL)
    {
        return NULL;
    }

    //Encontrar o nó com menor custo total
    No *menor_custo = p->topo;
    No *atual = p->topo->prox;
    while (atual != NULL)
    {
        //Verifica se o custo total do nó atual é menor que o custo do menor_custo
        if ((atual->estado.custo_atual + atual->estado.custo_heuristico) < (menor_custo->estado.custo_atual + menor_custo->estado.custo_heuristico))
        {
            menor_custo = atual;
        }

        atual = atual->prox;
    }
    
    
    //Copia a lista de produtos original, retirando os produtos já vistos pelos ancestrais do nó atual
    No *aux = menor_custo;
    char prodJaEscolhido;
    char produtos_restantes[strlen(produtos_originais) + 1];
    strcpy(produtos_restantes, produtos_originais);
    while (aux != NULL)
    {
        prodJaEscolhido = aux->estado.prodSup->produto.nome;
        char *ptr = strchr(produtos_restantes, prodJaEscolhido);
        if (ptr != NULL)
        {
            memmove(ptr, ptr + 1, strlen(ptr));
        }
        aux = aux->paiNo;
    }
    
    printf("\n-----------------------------------------------------------\n");
    printf("\nProdutos restantes: %s\n", produtos_restantes);

    //Se o nó de menor preço total é o ultimo produto da lista, chegou ao fim da busca A* (nenhum nó tem custo menor do que ele para ser viditado)
    if (produtos_restantes[0] == '\0')
    {
        return menor_custo;
    }
    
    printf("\nAbrindo nó com menor custo total:\n");
    printf("Produto: %c, Supermercado: %c, Custo Atual: %.2f, Custo Heurístico: %.2f\n\n",
           menor_custo->estado.prodSup->produto.nome,
           menor_custo->estado.prodSup->supermercado,
           menor_custo->estado.custo_atual,
           menor_custo->estado.custo_heuristico);

    //Adiciona os filhos do nó aberto (nó com menor custo total) à pilha
    adicionar_nos(p, menor_custo, lista_supermercados, produtos_restantes);
    //Remove o nó aberto da pilha
    remover_no(p, menor_custo);

    return NULL;
}



//Executa o algoritmo A* para encontrar a solução com o menor custo
void A_Estrela(Pilha *p, ListaSupermercados *lista_supermercados, char *produtos_originais)
{
    do
    {
        printf("\nProdutos escolhidos: %s\n", produtos_originais);

        //Adiciona os nós iniciais à pilha
        adicionar_nos(p, NULL, lista_supermercados, produtos_originais);
        imprimir_pilha(p);

        //Enquanto ainda há nós a serem verificados
        while (p->topo != NULL)
        {
            //Abre o nó com menor custo, adicionando seus filhos na pilha
            No *resultado = abrir_no_menor_custo(p, lista_supermercados, produtos_originais);

            //Se terminou a busca e encontrou a solução
            if (resultado != NULL)
            {
                printf("\nSolução Encontrada!\n");
                float custo_total = resultado->estado.custo_atual;
                float custo_frete = custo_total;
                //Obtem a solução acessando de forma iterativa os ancestrais do nó com o menor custo total (menor custo da compra inteira)
                while (resultado != NULL)
                {
                    char prodNome = resultado->estado.prodSup->produto.nome;
                    char supermercadoNome = resultado->estado.prodSup->supermercado;
                    printf("Produto %c no supermercado %c\n", prodNome, supermercadoNome);
                    custo_frete -= resultado->estado.prodSup->produto.preco;
                    resultado = resultado->paiNo;
                }
                printf("Preço total (com o frete): R$%.2f\n", custo_total);
                printf("Total de frete: R$%.2f", custo_frete);
                return;
            }
            
            imprimir_pilha(p);
        }
    }
    while (p->topo != NULL);
}