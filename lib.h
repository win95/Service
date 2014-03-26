#include <sys/socket.h>
#include <sys/types.h>

#include <ifaddrs.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <pthread.h>

#define PORT 5555
#define MAXMSG 1024

#define MSG1 "RESPONSE"
#define MSG2 "REQUEST"

char buff[20];
char msg_ip[32][32]; // se de luu ip vao de truyen

/*Hien thi ma loi va noi dung*/
void error(char *msg);
/*Lay thoi gian hien tai*/
char* now();
/*Ghi vao file*/
void write_file(char *file_name,char *msg);
/*Ghi vao file*/
void Replace_file(char *file_name,char *msg);
/*Check ip*/
int check_ip(char *file_name,char *ip);
/*Lay clock den microsecond*/
const char* clock_microsecond();
/*Get timezone*/
static int get_timezone(void);
/*Get file size*/
int get_file_size(FILE *fd);
/* listen port */

void read_ip(int i);

void soc_ip();