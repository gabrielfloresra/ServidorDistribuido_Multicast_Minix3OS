
#include <sys/ipc.h>
/**
 * 
 * ftok()
 * 
 **/

#include <sys/sem.h>
/**
 * 
 *  semop()
 *  semctl()
 * 
 **/

#include <sys/shm.h>
/**
 * 
 *  shmget()
 *  shmat()
 * 
 **/

void semAcquire(int semid)
{
    struct sembuf sops;
    sops.sem_num = 0; // posicion del arreglo de semaforos
    sops.sem_op = -1; // adquiere un permiso del semaforo
    sops.sem_flg = 0;

    if (semop(semid, &sops, 1) == -1)
    {
        perror("semop() acquire");
    }
}

void semRelease(int semid)
{
    struct sembuf sops;
    sops.sem_num = 0; // posicion del arreglo de semaforos
    sops.sem_op = 1;  // libera un permiso del semarofo
    sops.sem_flg = 0;

    if (semop(semid, &sops, 1) == -1)
    {
        perror("semop() release");
    }
}

int semCreate(int initValue)
{
    int sem;

    int clave;

    if ((clave = ftok("/bin/ls", initValue)) == -1)
    {
        perror("ftok");
    }

    if ((sem = semget(clave, 1, IPC_CREAT | 0777)) < 0)
    {
        perror("semget");
    }

    if (semctl(sem, 0, SETVAL, initValue) < 0)
    {
        perror("semctl createSem");
    }
    return sem;
}

void semRemove(int semaforo)
{
    if ((semctl(semaforo, 0, IPC_RMID)) == -1)
    {
        perror("semctl rmSem");
    }
}