#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

#include <stdio.h>
/**
 *
 *  printf()
 *  fgets() 
 * 
 **/

#include <stdlib.h>
/**
 *
 *  
 * 
 **/

#include "p2p.h"
#include "timer.h"
#include "sem.h"
#include "queue.h"

#define TAM_MSJ 50
#define TIME_TO_SET_COORDINADOR 1500
#define TIME_TO_ROLL_CALL_ASSISTEANCE 1000

int numAssistanceList;
int casoTimer,
    tipoPeer = -1,
    pid = -1;
Queue *list;
struct itimerval timer;
/**
 * 
 * como no-coordinador
 * 
 **/
int getNumAssistanceList();
void rcvMsjCordinador();
void setCoordiador();
void rollCallAssistance();

/**
 * 
 * como coordinador
 * 
 **/
void initCoordinador();
void sendMsjPeers();
void addPeer();
int assignNumAssistanceList();
void temp();
/**
 * 
 * 
 * 
 **/
void recvMsj_other(int *num, char *ipOrigen, char *ipDestino);
void recvMsj_other(int *num, char *ipOrigen, char *ipDestino)
{
    int cond = 1;
    char *msj = (char *)malloc(sizeof(char) * TAM_MAX_CAD);

    while (cond)
    {
        msj = recvMsj();
        /**
        *	cuando pasa determinado tiempo y no se recibe un mensaje
        *	la variable $msj toma el valor de la cadena vacia
        *
        */
        if (strcmp(msj, ""))
        {
            snprintf(ipOrigen, TAM_MAX_CAD, "%s", strtok(msj, "::"));
            if (strcmp(ipOrigen, localIP))
            {
                snprintf(ipDestino, TAM_MAX_CAD, "%s", strtok(NULL, "::"));
                (*num) = atoi(strtok(NULL, "::"));
                cond = 0;
            }
        }
        else
        {
            cond = 0;
        }
    }
}

/**
 * 
 * 
 * 
 **/
void terminarPrograma()
{
    if (pid != -1)
    {
        if (pid)
        {
            kill(SIGKILL, pid);
        }
    }
    printf(ANSI_COLOR_RESET);
    exit(1);
}

void manejador1()
{
    printf("hola\n");
}
/**
 * 
 * 
 * 
 **/
int main()
{
    //timer = setTimer(manejador1, 500);
    //int i = 1;
    //while (1)
    //{
    //    i = 1;
    //}
    //return 0;

    if (initP2P() == -1)
    {
        terminarPrograma();
    }

    signal(SIGINT, terminarPrograma);

    queueDelete(list);
    list = queueCreate();

    printf(ANSI_COLOR_RED "mi ip es:" ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, localIP);

    printf("=>obteniendo numero de lista\n");
    numAssistanceList = getNumAssistanceList();

    printf("tipo peer: %d\n", tipoPeer);
    while (tipoPeer >= 0)
    {
        if (!tipoPeer)
        {
            rcvMsjCordinador();
            setCoordiador();
        }
        else
        {
            initCoordinador();
        }
    }

    return 0;
}

/**
 * 
 * 
 * 
 **/
void rcvMsjCordinador()
{
    printf(ANSI_COLOR_GREEN "\t=>recibiendo mensajes del coordinador\n" ANSI_COLOR_RESET);
    char *ipOrigen = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
    char *ipDestino = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
    int num;

    timer = setTimer(temp, TIME_TO_SET_COORDINADOR);

    casoTimer = 1;
    int numMsj = 0;
    while (casoTimer)
    {
        recvMsj_other(&num, ipOrigen, ipDestino);
        if (casoTimer)
        {
            printf("coordinador%d: %s\n", ++numMsj, ipOrigen);
            resetTimer(timer, TIME_TO_SET_COORDINADOR);
        }
    }
}

/**
 * 
 * 
 * 
 **/
void setCoordiador()
{
    printf(ANSI_COLOR_RED "\t=>el coordinador murio\n" ANSI_COLOR_RESET);
    queueDelete(list);
    list = queueCreate();

    printf(ANSI_COLOR_RED "=>esperando para pasar lista...\n" ANSI_COLOR_RESET);
    sleep(1);
    printf(ANSI_COLOR_RED "=>pasando lista...\n" ANSI_COLOR_RESET);
    rollCallAssistance();

    int coordinador = list->first->num;
    if (numAssistanceList == coordinador)
    {
        printf(ANSI_COLOR_GREEN "=>soy el nuevo coordinador\n" ANSI_COLOR_RESET);
        tipoPeer = 1;
    }
}

/**
 * 
 * 
 * 
 **/
void temp()
{
    casoTimer = 0;
}
void rollCallAssistance()
{
    Nodo *nodo;

    int num;
    char *ipOrigen = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
    char *ipDestino = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
    char *msj = (char *)malloc(sizeof(char) * TAM_MAX_CAD);

    snprintf(msj, TAM_MAX_CAD, "-1::%s::%d", localIP, numAssistanceList);
    printf(ANSI_COLOR_GREEN "\t=>enviando status activo\n" ANSI_COLOR_RESET);
    sendMsj(msj);

    casoTimer = 1;
    printf(ANSI_COLOR_GREEN "\t=>pasando lista\n" ANSI_COLOR_RESET);

    timer = setTimer(temp, TIME_TO_ROLL_CALL_ASSISTEANCE);
    while (casoTimer)
    {
        recvMsj_other(&num, ipOrigen, ipDestino);
        if (casoTimer)
        {
            nodo = nodoCreate(TAM_MAX_CAD, ipDestino, num);
            printf("=>peer Activo %s : %d\n", ipDestino, num);
            queuePush(list, nodo);
            resetTimer(timer, TIME_TO_ROLL_CALL_ASSISTEANCE);
        }
    }
}

/**
 * 
 * 
 * 
 **/
int getNumAssistanceList()
{
    int num = -1;
    char *ipOrigen = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
    char *ipDestino = (char *)malloc(sizeof(char) * TAM_MAX_CAD);

    char *msj = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
    snprintf(msj, TAM_MAX_CAD, "%s::-1::%d", localIP, num);

    sendMsj(msj);

    timer = setTimer(temp, TIME_TO_SET_COORDINADOR);

    casoTimer = 1;
    while (casoTimer)
    {
        recvMsj_other(&num, ipOrigen, ipDestino);
        if (!casoTimer)
        {
            tipoPeer = 1;
            printf(ANSI_COLOR_RED "\t=>no existe ningun coordinador\n");
        }
        else
        {
            if (!strcmp(ipDestino, localIP))
            {
                numAssistanceList = num;
                casoTimer = 0;
                printf(ANSI_COLOR_RED "=>el coordinador: " ANSI_COLOR_GREEN "%s" ANSI_COLOR_RED "\nme asigno el numero: " ANSI_COLOR_GREEN "%d\n" ANSI_COLOR_RESET, ipOrigen, numAssistanceList);
                tipoPeer = 0;
                //pauseTimer(timer);
            }
        }
    }

    return num;
}

/**
 * 
 * 
 * 
 **/
void initCoordinador()
{
    printf(ANSI_COLOR_GREEN "\t=>soy el nuevo coordinador\n" ANSI_COLOR_RESET);
    if ((pid = fork()) < 0)
    {
        perror("g.c :: fork() :: 91");
        terminarPrograma();
    }

    if (pid)
    {
        printf(ANSI_COLOR_GREEN "=>enviando mensajes a los peers\n" ANSI_COLOR_RESET);
        sendMsjPeers();
    }
    else
    {
        addPeer();
    }
}

/**
 * 
 * 
 * 
 **/
void sendMsjPeers()
{
    char *msj = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
    snprintf(msj, TAM_MAX_CAD, "%s::-1::%d", localIP, numAssistanceList);

    while (1)
    {
        sendMsj(msj);
        sleep(1);
    }
}

/**
 * 
 * 
 * 
 **/
void addPeer()
{
    char *ipOrigen = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
    char *ipDestino = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
    char *msj = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
    int num;

    while (1)
    {
        printf("=>esperando para agregar peers\n");
        recvMsj_other(&num, ipOrigen, ipDestino);
        printf(ANSI_COLOR_GREEN "\n\t=>asignando numero de lista a %s\n" ANSI_COLOR_RESET, ipOrigen);
        int num = assignNumAssistanceList();

        printf(ANSI_COLOR_GREEN "\t=>numero asignado %d\n" ANSI_COLOR_RESET, num);
        Nodo *new = nodoCreate(TAM_MAX_CAD, ipOrigen, num);
        queuePush(list, new);

        snprintf(msj, TAM_MAX_CAD, "%s::%s::%d", localIP, ipOrigen, num);
        sendMsj(msj);
    }
}

/**
 * 
 * 
 * 
 **/
int assignNumAssistanceList()
{
    int num = 1;

    Nodo *last;
    if ((last = queuePopLast(list)))
    {
        num = last->num + 1;
    }

    return num;
}