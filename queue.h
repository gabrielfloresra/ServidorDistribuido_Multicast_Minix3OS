/***
 * 
 *
 *  sorted Queue 
 * 
 * 
 ***/
#include <stdio.h>
#include <stdlib.h>
/**
 * 
 * malloc()
 * 
 **/

/**
 * 
 * 
 * 
 **/
typedef struct Nodo
{
    char *cad;
    int num;
    struct Nodo *next;
} Nodo;

/**
 * 
 * 
 * 
 **/
typedef struct Queue
{
    int size;
    Nodo *first;
    Nodo *last;
} Queue;

/**
 * 
 * 
 * 
 **/
Nodo *nodoCreate(int, char *, int);
Queue *queueCreate();
void queuePush(Queue *, Nodo *);
Nodo *queuePop(Queue *);
Nodo *queuePopLast(Queue *);
void queueDelete(Queue *);
int getSizeQueue(Queue *);
int queueEmpty(Queue *);
void printQueue(Queue *cola);

/**
 * 
 * 
 * 
 **/
Nodo *nodoCreate(int lenCad, char *value, int num)
{
    Nodo *nodo = (Nodo *)malloc(sizeof(Nodo));

    nodo->cad = (char *)malloc(lenCad * (sizeof(char)));
    snprintf(nodo->cad, lenCad, "%s", value);
    nodo->num = num;

    nodo->next = NULL;
    return nodo;
}

/**
 * 
 * 
 * 
 **/

/**
 * 
 * 
 * 
 **/
Queue *queueCreate()
{
    Queue *cola = (Queue *)malloc(sizeof(Queue));
    cola->first = NULL;
    cola->size = 0;
    return cola;
}

/**
 * 
 * 
 * 
 **/
void queuePush(Queue *cola, Nodo *nodo)
{
    cola->size = cola->size + 1;
    if (cola->first == NULL)
    {
        cola->first = nodo;
        cola->last = nodo;
    }
    else
    {
        Nodo *after = cola->first;
        Nodo *before = NULL;

        while (after != NULL && (nodo->num > after->num))
        {
            before = after;
            after = after->next;
        }
        if (after == NULL)
        {
            //printf("insertado al final de la cola\n");
            cola->last = nodo;
            before->next = nodo;
        }
        else if (before == NULL)
        {
            //printf("insertado al inicio de la cola\n");
            nodo->next = after;
            cola->first = nodo;
        }
        else
        {
            //printf("insertado en la parte media\n");
            before->next = nodo;
            nodo->next = after;
        }
    }
}

/**
 * 
 * 
 * 
 **/
Nodo *queuePop(Queue *cola)
{
    Nodo *deleted = NULL;
    if (queueEmpty(cola))
    {
        printf("empty queue\n");
        return deleted;
    }

    deleted = cola->first;
    (cola->size)--;
    cola->first = cola->first->next;
    return deleted;
}

/**
 * 
 * corregir
 * 
 **/
Nodo *queuePopLast(Queue *cola)
{
    Nodo *last = NULL;
    if (queueEmpty(cola))
    {
        printf("empty queue\n");
        return last;
    }

    last = cola->last;
    return last;
}

/**
 * 
 * 
 * 
 **/

/**
 * 
 * 
 * 
 **/
void queueDelete(Queue *cola)
{
    if (cola != NULL)
    {
        while (cola->first != NULL)
        {
            free(queuePop(cola));
        }
        free(cola);
    }
}

/**
 * 
 * 
 * 
 **/
int getSizeQueue(Queue *cola)
{
    return cola->size;
}

/**
 * 
 * 
 * 
 **/

/**
 * 
 * 
 * 
 **/
int queueEmpty(Queue *cola)
{
    int caso = 0;
    if (cola == NULL || (cola != NULL && cola->first == NULL))
    {
        caso = 1;
    }
    return caso;
}

/**
 * 
 * 
 * 
 **/
void printQueue(Queue *cola)
{
    if (queueEmpty(cola))
    {
        printf("empty queue\n");
        return;
    }

    Nodo *aux = cola->first;

    while (aux != NULL && aux->cad != NULL)
    {
        printf("%d :: %s\n", aux->num, aux->cad);
        aux = aux->next;
    }
}