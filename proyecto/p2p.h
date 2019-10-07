#include <sys/socket.h>
/**
 * 
 *  socket()
 *  setsockopt()
 *  bind()
 *  recvfrom()
 *  sendto()
 * 
 **/

#include <netinet/in.h>
/**
 * 
 *  IPPROTO_IP
 *  sockaddr_in
 *  htons()
 *  htonl()
 * 
 **/

#include <arpa/inet.h>
/**
 * 
 *  inet_addr()
 * 
 **/

#include <string.h>
/**
 * 
 *  bzero()
 * 
 **/

/**
 * 
 * 
 * 
 **/
#define PUERTO 2333
#define IP "233.3.3.3"
#define TAM_MAX_CAD 2048

/**
 * 
 * 
 * 
 **/
struct sockaddr_in addrSend,
    addrRecv;
char localIP[TAM_MAX_CAD];
int addrlen,
    sock;

/**
 * 
 * 
 * 
 **/
struct sockaddr_in getSockAddr();
void sendMsj(char *msj);
char *recvMsj();
int initP2P();
void setLocalIP();

int setSockOptions()
{
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(IP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY); //The address of the IPv4 multicast group
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) < 0)
    {
        perror("setsockopt, IP_ADD_MEMBERSHIP");
        return -1;
    }

    unsigned int yes = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0)
    {
        perror("setsockopt, SO_REUSEADDR");
        return -1;
    }
    return 1;
}
/**
 * 
 * 
 * 
 **/
int initP2P()
{
    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("p2p.h :: socket :: 78");
        return -1;
    }

    setLocalIP();
    //setSockOptions();

    addrSend = getSockAddr(1);
    addrRecv = getSockAddr(0);
    addrlen = sizeof(addrRecv);

    if (bind(sock, (struct sockaddr *)&addrRecv, addrlen) < 0)
    {
        perror("p2p.h :: bind :: 86");
        return -1;
    }

    return 1;
}

/**
 * 
 * 
 * 
 **/
void setLocalIP()
{

#ifdef __minix__
#define comando "ifconfig"
    FILE *fd = popen(comando, "r");
    char aux[100];
    char *cad;
    fgets(aux, 100, fd);
    cad = strtok(aux, " ");
    cad = strtok(NULL, " ");
    cad = strtok(NULL, " ");
    snprintf(localIP, 50, "%s", cad);
#else
#define comando "ip -4 address | grep -o -E '[0-9]+.[0-9]+.[0-9]+.[0-9]+' | grep -v 255$ | grep -v 127"
    FILE *fd = popen(comando, "r");
    char aux[100];
    char *cad;
    fgets(aux, 100, fd);
    aux[strlen(aux) - 1] = '\0';
    snprintf(localIP, 50, "%s", aux);
#endif
}

/**
 * 
 * 
 * 
 **/
struct sockaddr_in getSockAddr(int caso)
{
    struct sockaddr_in addr;
    if (caso)
    { //send
        addr.sin_addr.s_addr = inet_addr(IP);
    }
    else
    { //recv
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PUERTO);
    bzero(&(addr.sin_zero), 8);

    return addr;
}

/**
 * 
 * 
 * 
 **/
char *recvMsj()
{
    char *msj = (char *)malloc(sizeof(char) * TAM_MAX_CAD);

    if (recvfrom(sock, msj, TAM_MAX_CAD, 0, (struct sockaddr *)&addrRecv, (socklen_t *)&addrlen) < 0)
    {
        //perror("recvfrom");
    }

    return msj;
}

/**
 * 
 * 
 * 
 **/
void sendMsj(char *msj)
{
    if (sendto(sock, msj, TAM_MAX_CAD, 0, (struct sockaddr *)&addrSend, addrlen) < 0)
    {
        perror("sendto");
    }
}