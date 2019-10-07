#include <sys/time.h>
/**
 * 
 *  setitimer()
 * 
 **/

#include <unistd.h>
/**
 * 
 *  pause()
 *  fork()
 * 
 **/

#include <signal.h>
/**
 * 
 *  signal()
 * 
 **/

struct itimerval setTimer(void manejador(int), int intervalMiliSec);
int resetTimer(struct itimerval timer, int intervalMiliSec);
int pauseTimer(struct itimerval timer);
int resumeTimer(struct itimerval timer);

struct itimerval setTimer(void manejador(), int intervalMiliSec)
{
    struct itimerval timer;

    if (signal(SIGALRM, manejador) == SIG_ERR)
    {
        perror("signal()\n");
        return timer;
    }

    //struct sigaction sSigAction;
    //sSigAction.sa_flags = SA_RESTART;
    //sSigAction.sa_sigaction = manejador;
    //sigemptyset(&sSigAction.sa_mask);
    //sigaction(SIGPROF, &sSigAction, NULL);

    resetTimer(timer, intervalMiliSec);
    return timer;
}

int pauseTimer(struct itimerval timer)
{
    struct itimerval zero_timer = {0};
    if (setitimer(ITIMER_REAL, &zero_timer, &timer) == -1)
    {
        perror("timer.h :: setitimer() _ pauseTimer :: 44");
        return -1;
    }
    return 1;
}

int resetTimer(struct itimerval timer, int intervalMiliSec)
{
    int sec = intervalMiliSec / 1000;
    int mil = intervalMiliSec % 1000;
    timer.it_value.tv_sec = sec;
    timer.it_value.tv_usec = mil * 1000;
    timer.it_interval = timer.it_value;
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
    {
        perror("timer.h :: setitimer() _ resetTimer :: 56");
        return -1;
    }
    return 1;
}

int resumeTimer(struct itimerval timer)
{
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1)
    {
        perror("timer.h :: setitimer() _ resumeTimer :: 65");
        return -1;
    }
    return 1;
}