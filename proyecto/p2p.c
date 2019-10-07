#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h> //socket(), setsockopt(), bind(), recvfrom(), sendto()
#include <netinet/in.h> //IPPROTO_IP, sockaddr_in, htons(), htonl()
#include <arpa/inet.h>  //inet_addr()

#include <errno.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h> //Memoria compartida

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sem.h"

#define PUERTO 2333
#define IP "233.3.3.3"
#define TAM_MAX_CAD 2048

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

/**
 * 
 * 
 * 
 * 
 * 
 * 
 **/
char IPLocal[50];

struct sockaddr_in addr;
int pid1,
    pid2,
    sock,
    semAccess_theFile;
/**
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 **/
void getRequest_toWrite();
void sendRequest_toWrite();
void write_toFile();
void write_toFileHandler();
void readFile();
void setInterval_toWrite();
int getRandomNum();
/**
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 **/
struct sockaddr_in
getSockAddr(int puerto, char *ip)
{
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(puerto);
    bzero(&(addr.sin_zero), 8);

    return addr;
}

void terminaPrograma()
{
    printf(ANSI_COLOR_RESET);
    exit(0);
}
void imprimeIPLocal()
{
    FILE *fp = popen("ifconfig", "r");

    char cad[50];
    char cad1[100] = "";
    while (fgets(cad, 50, fp) != NULL)
        sprintf(cad1, "%s%s", cad1, cad);

    char *aux;

    aux = strtok(cad1, " ");
    aux = strtok(NULL, " ");
    aux = strtok(NULL, " ");

    sprintf(IPLocal, "%s", aux);
    printf(ANSI_COLOR_RED "\n\nIP Local: " ANSI_COLOR_GREEN "_%s_\n\n" ANSI_COLOR_RESET, IPLocal);
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
int main(int argc, char const *argv[])
{
    system("clear");
    FILE *f = fopen("./texto.txt", "w");
    fclose(f);

    srand(time(NULL));

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("socket");
        terminaPrograma();
    }

    /**
     * 
     * crea el semaforo y lo inicializa en 1
     * 
     **/
    semAccess_theFile = semCreate(1);
    /**
     * 
     * 
     * 
     * 
     **/
    addr = getSockAddr(PUERTO, IP);
    signal(SIGINT, terminaPrograma);
    signal(SIGSEGV, terminaPrograma);

    /**
     * 
     * 
     * 
     * 
     * 
     * 
     **/
    imprimeIPLocal();

    if ((pid1 = fork()) < 0)
    {
        exit(0);
    }
    if((pid1) == 0){
        if((pid2 = fork()) < 0){
            kill(SIGKILL, pid1);
            exit(0);
        }
    }
    /**
     * 
     * 
     * 
     * 
     * 
     * 
     **/
    if (pid1 == 0)
    {
      if(pid2 == 0){
        readFile();
      }
      else{
        signal(SIGALRM, sendRequest_toWrite);
        setInterval_toWrite();
        write_toFileHandler();
      }
    }
    else
    {
      getRequest_toWrite();
    }

    return 0;
}

int getRandomNum()
{
    int mayor = 30;
    int menor = 10;
    return rand() % (mayor - menor + 1) + menor;
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

void getRequest_toWrite()
{
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        terminaPrograma();
    }

    char mensaje[TAM_MAX_CAD];
    int addrlen = sizeof(addr);
    while (1)
    {
        if (recvfrom(sock, mensaje, TAM_MAX_CAD, 0, (struct sockaddr *)&addr, (socklen_t *)&addrlen) < 0)
        {
            perror("recvfrom");
            terminaPrograma();
        }
        semAcquire(semAccess_theFile);
        printf(ANSI_COLOR_RED "=>peticion para escribir recibida\n" ANSI_COLOR_RESET);
        write_toFile(mensaje);
        printf(ANSI_COLOR_GREEN "=>liberando\n" ANSI_COLOR_RESET);
        semRelease(semAccess_theFile);
    }
}

void write_toFile(char *cad)
{
    printf("=>escribiendo al fichero: %s\n", cad);
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
 * 
 **/
void setInterval_toWrite()
{
  int num;
  num = getRandomNum();
  printf(ANSI_COLOR_RED "\t=>escribiendo en %d seg.\n" ANSI_COLOR_RESET, num);
  alarm(num);
}

void write_toFileHandler()
{
  while(1)
  {
    sleep(1);
  }
}

void sendRequest_toWrite()
{
  semAcquire(semAccess_theFile);
  char mensaje[TAM_MAX_CAD];

  snprintf(mensaje, TAM_MAX_CAD, "soy: %s, saludando a los demas peers\n", IPLocal);
  printf(ANSI_COLOR_GREEN "\t=>enviando peticion para escribir\n" ANSI_COLOR_RESET);
  if (sendto(sock, mensaje, sizeof(mensaje), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    perror("sendto");
    terminaPrograma();
  }
  semRelease(semAccess_theFile);
  sleep(1);
  semAcquire(semAccess_theFile);
  printf(ANSI_COLOR_GREEN "\testableciendo manejadorEscritura\n" ANSI_COLOR_RESET);
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
        }
        else
        {
            printf(ANSI_COLOR_RED "\t=>cadLeida:" ANSI_COLOR_GREEN " %s\n" ANSI_COLOR_RESET, cad);
        }
        semRelease(semAccess_theFile);
        sleep(1);
    }
}