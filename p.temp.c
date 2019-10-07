
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

#define TIME_TO_SET_COORDINADOR 1500
#define TIME_TO_ROLL_CALL_ASSISTEANCE 1000

/**
 *
 * variables para el programa de Lectores / Escritores
 *
 **/
int semAccess_theFile;

/**
 *
 * funciones para el programa de Lectores / Escritores
 *
 **/
int getRandomNum();
void setInterval_toWrite();
void readFile();
void write_toFileHandler();
void write_toFile(char *cad);
void sendRequest_toWrite();


/**
 *
 *
 * programa grandulon 
 *
 *
 **/

int numAssistanceList;
int casoTimer = 1,
    tipoPeer = -1;
Queue *list;
struct itimerval timer;
char *ipOrigen;
char *destino;
char *peticion;
char *mensaje;

/**
 * 
 * como no-coordinador
 * 
 **/
void set_NumAssistanceList();
void rcvMsjCordinador();
void setCoordinador();
void rollCallAssistance();

/**
 * 
 * como coordinador
 * 
 **/
void initCoordinador();
void sendMsjPeers();
void addPeer();
int assign_NumAssistanceList();
void temp();

#define MSJ_REQUEST_TO_WRITE "peticion a la red para escribir al fichero"
#define MSJ_REQUEST_TO_JOIN "peticion de un nuevo peer para unirse a la red"
#define MSJ_I_AM_ACTIVE "peer activo"

/**
 * 
 * 
 * 
 **/
void msjRecv_Split(char *ipO, char *dest, char *msjReq, char *msjCont);
void msjRecv_Split(char *ipO, char *dest, char *msjReq, char *msjCont)
{
  casoTimer = 1;

  char *msj = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
  msj = recvMsj();

  if (strcmp( msj, ""))
  {
    snprintf( ipO, TAM_MAX_CAD, "%s", strtok(msj, "::"));
    //if (strcmp(ipO, localIP))
    //{
    snprintf( dest, TAM_MAX_CAD, "%s", strtok(NULL, "::"));
    snprintf( msjReq, TAM_MAX_CAD, "%s", strtok(NULL, "::"));
    snprintf( msjCont, TAM_MAX_CAD, "%s", strtok(NULL, "::"));
    //}
  } else {
    casoTimer = 0;
  }
}

/**
 * 
 * 
 * 
 **/
void terminarPrograma()
{

  /**
   *
   * Block comment
   *
    if (pid != -1)
    {
      if (pid)
      {
        kill(SIGKILL, pid);
      }
    }
   */
  
  printf(ANSI_COLOR_RESET);
  exit(1);
}

/**
 * 
 * 
 * 
 **/
int main()
{
  system("clear");
  FILE *f = fopen("./texto.txt", "w");
  fclose(f);
  semAccess_theFile = semCreate(1);

  if (initP2P() == -1)
  {
      terminarPrograma();
  }

  ipOrigen = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
  destino = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
  peticion = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
  mensaje = (char *)malloc(sizeof(char) * TAM_MAX_CAD);
  signal(SIGINT, terminarPrograma);
  signal(SIGSEGV, terminarPrograma);

  printf(ANSI_COLOR_RED "mi ip es:" ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET, localIP);

  printf("=>obteniendo numero de lista\n");
  set_NumAssistanceList();

  queueDelete(list);
  list = queueCreate();

  while (tipoPeer >= 0)
  {
    if (!tipoPeer)
    {
      rcvMsjCordinador();
      setCoordinador();
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
  int pid_Recv_Msj;
  int pid_ReadFile;
  if ( (pid_Recv_Msj = fork()) < 0)
  {
      perror("g.c :: fork() :: 321");
      terminarPrograma();
  }

  if (pid_Recv_Msj == 0) {
    if ((pid_ReadFile = fork()) < 0)
    {
      perror("g.c :: fork() :: 321");
      terminarPrograma();
    }
  }

  if (pid_Recv_Msj)
  {
    casoTimer = 1;
    printf(ANSI_COLOR_GREEN "\t=>recibiendo mensajes del coordinador\n" ANSI_COLOR_RESET);

    timer = setTimer(temp, TIME_TO_SET_COORDINADOR);

    int numMsj = 0;
    while (casoTimer)
    {
      msjRecv_Split( ipOrigen, destino, peticion, mensaje);
      if (casoTimer)
      {
        semAcquire( semAccess_theFile);

        if( !strcmp(peticion, MSJ_REQUEST_TO_JOIN) ){

        } else if ( !strcmp(peticion, MSJ_REQUEST_TO_WRITE)) {

            printf(ANSI_COLOR_BLUE "=>peticion para escribir recibida, casoTimer: %d\n" ANSI_COLOR_RESET, casoTimer);
            write_toFile(mensaje);
            printf(ANSI_COLOR_GREEN "=>liberando\n" ANSI_COLOR_RESET);
            casoTimer = 1;
        } else if ( !strcmp(peticion, MSJ_I_AM_ACTIVE)) {

            printf("coordinador[%d]: %s\n", ++numMsj, ipOrigen);
            resetTimer(timer, TIME_TO_SET_COORDINADOR);
        }
        semRelease( semAccess_theFile);
      }
    }
  } else {
    if (pid_ReadFile) {

      readFile();
    } else {

      signal(SIGALRM, sendRequest_toWrite);
      setInterval_toWrite();
      write_toFileHandler();
    }
  }
}

/**
 * 
 * 
 * 
 **/
void setCoordinador()
{
  semAcquire( semAccess_theFile);
  printf(ANSI_COLOR_RED "\t=>el coordinador murio\n" ANSI_COLOR_RESET);
  queueDelete(list);
  list = queueCreate();

  printf(ANSI_COLOR_RED "=>esperando para pasar lista...\n" ANSI_COLOR_RESET);
  sleep(1);
  printf(ANSI_COLOR_RED "=>pasando lista...\n" ANSI_COLOR_RESET);
  rollCallAssistance();

  int coordinador = list->first->num;
  printf("nuevo coordinador: %d\n", coordinador);
  printf(":: %d\n", numAssistanceList);
  if (numAssistanceList == coordinador)
  {
    printf(ANSI_COLOR_GREEN "=>soy el nuevo coordinador\n" ANSI_COLOR_RESET);
    tipoPeer = 1;
  }
  semRelease( semAccess_theFile);
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

  snprintf( mensaje, TAM_MAX_CAD, "%s::all::%s::%d", localIP, MSJ_I_AM_ACTIVE, numAssistanceList);
  printf(ANSI_COLOR_GREEN "\t=>enviando status activo\n" ANSI_COLOR_RESET);
  sendMsj( mensaje);

  printf(ANSI_COLOR_GREEN "\t=>pasando lista\n" ANSI_COLOR_RESET);

  timer = setTimer(temp, TIME_TO_ROLL_CALL_ASSISTEANCE);
  printf("esperando...%d\n", casoTimer);
  casoTimer = 1;
  while (casoTimer)
  {
    msjRecv_Split( ipOrigen, destino, peticion, mensaje);
    if (casoTimer)
    {
      nodo = nodoCreate(TAM_MAX_CAD, ipOrigen, atoi(mensaje));
      printf("=>peer Activo %s : %s\n", ipOrigen, mensaje);
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
void set_NumAssistanceList()
{
  snprintf(mensaje, TAM_MAX_CAD, "%s::coordinador::%s::requiero acceso a la red", localIP, MSJ_REQUEST_TO_JOIN);
  sendMsj(mensaje);

  timer = setTimer(temp, TIME_TO_SET_COORDINADOR);

  while (casoTimer)
  {
    msjRecv_Split( ipOrigen, destino, peticion, mensaje);
    //printf(ANSI_COLOR_GREEN "\n\t=>mensaje recibido de: %s \ndestino: %s \npeticion: %s \nmensaje: %s\n" ANSI_COLOR_RESET, ipOrigen, destino, peticion ,mensaje);
    if (!casoTimer)
    {
      tipoPeer = 1;
      printf(ANSI_COLOR_RED "\t=>no existe ningun coordinador\n");
    }
    else
    {
      if ( !strcmp(destino, localIP) )
      {
        numAssistanceList = atoi(mensaje);
        casoTimer = 0;
        printf(ANSI_COLOR_RED "=>el coordinador: " ANSI_COLOR_GREEN "%s" ANSI_COLOR_RED "\nme asigno el numero: " ANSI_COLOR_GREEN "%d\n" ANSI_COLOR_RESET, ipOrigen, numAssistanceList);
        tipoPeer = 0;
        //pauseTimer(timer);
      }
    }
  }
}

/**
 * 
 * 
 * 
 **/
void initCoordinador()
{
    int pid_SendMsjPeers;
    int pid_AddPeer;
    int pid_ReadFile;
    printf(ANSI_COLOR_GREEN "\t=>soy el nuevo coordinador\n" ANSI_COLOR_RESET);
    if ( (pid_SendMsjPeers = fork()) < 0)
    {
        perror("g.c :: fork() :: 321");
        terminarPrograma();
    }

    if (pid_SendMsjPeers == 0) {
      if ((pid_AddPeer = fork()) < 0)
      {
        perror("g.c :: fork() :: 321");
        terminarPrograma();
      }
      if (pid_AddPeer == 0) {
        if ((pid_ReadFile = fork()) < 0)
        {
          perror("g.c :: fork() :: 321");
          terminarPrograma();
        }
      }
    }

    if (pid_SendMsjPeers)
    {
        printf(ANSI_COLOR_GREEN "=>enviando mensajes a los peers\n" ANSI_COLOR_RESET);
        sendMsjPeers();
    }
    else
    {
        if (pid_AddPeer) {

          addPeer();
        } else {

          if (pid_ReadFile) {

            readFile();
          } else {

            signal(SIGALRM, sendRequest_toWrite);
            setInterval_toWrite();
            write_toFileHandler();
          }
        }
    }
}

/**
 * 
 * 
 * 
 **/
void sendMsjPeers()
{
  snprintf(mensaje, TAM_MAX_CAD, "%s::all::%s::%d", localIP, MSJ_I_AM_ACTIVE, numAssistanceList);
  while (1)
  {
    sendMsj(mensaje);
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
  int num;

  while (1)
  {
    msjRecv_Split( ipOrigen, destino, peticion, mensaje);

    // añade un peer a la red

    semAcquire(semAccess_theFile);
    //printf(ANSI_COLOR_GREEN "\n\t=>mensaje recibido de: %s \ndestino: %s \npeticion: %s \nmensaje: %s\n" ANSI_COLOR_RESET, ipOrigen, destino, peticion ,mensaje);
    if( !strcmp(peticion, MSJ_REQUEST_TO_JOIN) && strcmp(ipOrigen, localIP)){

      printf("=>añadiendo peer\n");
      printf(ANSI_COLOR_GREEN "\n#####################################\n" ANSI_COLOR_RESET);
      printf(ANSI_COLOR_GREEN "#####################################\n\n" ANSI_COLOR_RESET);
      printf(ANSI_COLOR_GREEN "\n\t=>asignando numero de lista a %s\n" ANSI_COLOR_RESET, ipOrigen);
      num = assign_NumAssistanceList();

      printf(ANSI_COLOR_GREEN "\t=>num_Asignado %d\n" ANSI_COLOR_RESET, num);
      printf(ANSI_COLOR_GREEN "\n\n#####################################\n" ANSI_COLOR_RESET);
      printf(ANSI_COLOR_GREEN "#####################################\n\n" ANSI_COLOR_RESET);
      Nodo *new = nodoCreate(TAM_MAX_CAD, ipOrigen, num);
      queuePush(list, new);

      snprintf( mensaje, TAM_MAX_CAD, "%s::%s::%s::%d", localIP, ipOrigen, MSJ_REQUEST_TO_JOIN, num);
      sendMsj( mensaje);
      printf("=>peer añadido a la lista\n");
    } else if ( !strcmp(peticion, MSJ_REQUEST_TO_WRITE)) {

      printf(ANSI_COLOR_BLUE "=>peticion para escribir recibida\n" ANSI_COLOR_RESET);
      write_toFile(mensaje);
      printf(ANSI_COLOR_GREEN "=>liberando\n" ANSI_COLOR_RESET);
    } else if ( !strcmp(peticion, MSJ_I_AM_ACTIVE)) {

    }
    semRelease(semAccess_theFile);
  }
}

/**
 * 
 * 
 * 
 **/
int assign_NumAssistanceList()
{
    int num = 1;

    Nodo *last;
    if ((last = queuePopLast(list)))
    {
        num = last->num + 1;
    }

    return num;
}



/**
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 **/
void write_toFileHandler()
{
  while(1)
  {
    sleep(1);
  }
}

void write_toFile(char *cad)
{
    printf("=>escribiendo al fichero: %s", cad);
    FILE *f = fopen("./texto.txt", "a+");
    fputs(cad, f);
    fclose(f);
    sleep(3);
}

/**
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 **/
void sendRequest_toWrite()
{
  char mensaje[TAM_MAX_CAD];
  semAcquire(semAccess_theFile);

  snprintf(mensaje, TAM_MAX_CAD, "%s::all::%s::soy %s, saludando a los demas peers\n" ANSI_COLOR_RESET, localIP, MSJ_REQUEST_TO_WRITE, localIP);
  printf(ANSI_COLOR_CYAN "\n\n===>enviando peticion para escribir\n" ANSI_COLOR_RESET);
  sendMsj( mensaje);

  semRelease(semAccess_theFile);
  sleep(1);
  semAcquire(semAccess_theFile);

  printf(ANSI_COLOR_GREEN "\n\n===>estableciendo manejador_Escritura\n" ANSI_COLOR_RESET);
  signal(SIGALRM, sendRequest_toWrite);
  setInterval_toWrite();

  semRelease(semAccess_theFile);
}

/**
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 **/
void readFile()
{
  FILE *f = fopen("./texto.txt", "r");

  char cad[TAM_MAX_CAD];

  while (1)
  {
    semAcquire(semAccess_theFile);
    if (!fgets(cad, TAM_MAX_CAD, f))
    {
      rewind(f);
      //printf(ANSI_COLOR_RED "\t===>sin datos!!!\n" ANSI_COLOR_RESET);
    }
    else
    {
      printf(ANSI_COLOR_RED "\t=>cadLeida:" ANSI_COLOR_GREEN " %s" ANSI_COLOR_RESET, cad);
    }
    semRelease(semAccess_theFile);
    sleep(2);
  }
}

/**
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 **/
void setInterval_toWrite()
{
  int num;
  num = getRandomNum();
  printf(ANSI_COLOR_YELLOW "===>escribiendo en %d seg.\n\n" ANSI_COLOR_RESET, num);
  alarm(num);
}

int getRandomNum()
{
    int mayor = 30;
    int menor = 20;
    return rand() % (mayor - menor + 1) + menor;
}