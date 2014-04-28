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
#define PORT 	5555
#define MAXMSG 1024

#define MSG1 "RESPONSE"
#define MSG2 "REQUESTE"
#define MSG3 "OK"
#include <unistd.h> 
#include <signal.h>

unsigned int alarm(unsigned int seconds);

    char host[NI_MAXHOST];
    char broad[NI_MAXHOST];
    char msg_name [8];
    char msg1[32];
    char msg2[32];
char msg_ip[32][32];// se de luu ip vao de truyen
    
    int 	out = 0;
    int 	gw;
    int n,i;
    int broadcast = 1;
    int sockfd = 0;
    int sockfd1 = 0; 
   
    struct ifaddrs 	*ifaddr,*ifa;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clie_addr;
    
struct node
{
    int 	data;
    char 	*name;
    char 	*ip;
    struct node *next;
}*hear;

    struct node *nod;


// bien danh cho thread 
    pthread_t waiting_thread;
    pthread_t killing_thread;
    pthread_t updateip_thread;	    
    FILE *fd;
    
    void  broadcast_request_all_interface(int k);
    void *waiting_handle(void *t);
    void *killing_handle(void *t);
    void dc_updateip_handler(int sig);
    void *dc_update_ip(void *t);    
    void send_broadcast(char *bcat,char *s);
/*Hien thi ma loi va noi dung*/
void error(char *msg);
/*Lay thoi gian hien tai*/
char* now();
/*Lay clock den microsecond*/
const char* clock_microsecond();
/*Get timezone*/
static int get_timezone(void);
/* listen port */

void name(char *file_name);

    void soc_ip(struct node *r); 
/* ham danh cho linker list*/
    void duc_add(char *name,char *ip);
    // su dung phan nay
    void  duc_display(struct node *r);
    int duc_check(char *ip,struct node *rc);		// check ip trong link 
    void duc_delete_all();
    int count_link();
    void duc_links_arr(char *line);
    int dc_catch_signal(int sig, void (*handler)(int));
/*Ham phuc vu thread waiting*/
 void *waiting_handle(void *t){
  int sockfd = 0;
  int i;
  char msg_rec[32][32]; // se de luu ip vao de truyen 
  char *msg_dc;
  size_t size;
  struct sockaddr_in serv_addr;
  struct sockaddr_in clie_addr;
  
  if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0) {
    error("Khong tao dc socket");
  }
  
  memset(&serv_addr, '0', sizeof(serv_addr));
  memset(&clie_addr, '0', sizeof(clie_addr));
  memset(msg_rec,'0',sizeof(msg_rec));
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(PORT);
  
  int reuse = 1;
  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char *)&reuse,sizeof(int)) < 0 ) error("cant reuse port");
  
  if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0) {
    error("Khong bind dc socket");
  }
  
  fprintf(stdout,"Waiting for client...\n");
  
  size = sizeof(clie_addr);
  while(!out) {     
    if(recvfrom(sockfd,msg_rec,sizeof(msg_rec),0,(struct sockaddr*)&clie_addr,&size)<0) {
      error("Khong nhan dc ban tin");
    }    
    if (strstr(&msg_rec[0][0],"REQUESTE")) {
      fprintf(stdout,"%s\n",clock_microsecond());
      fprintf(stdout,"%s From: \nIP: %s\n\n",&msg_rec[0][0],inet_ntoa(clie_addr.sin_addr));
      clie_addr.sin_port = htons(PORT);
      
	  if(duc_check(inet_ntoa(clie_addr.sin_addr),nod)==0) 
	  {
	    printf("ghi vao links :%s",inet_ntoa(clie_addr.sin_addr));
	    duc_add(&msg_rec[0][8],inet_ntoa(clie_addr.sin_addr));
	    printf("%s",&msg_rec[0][8]);
	    printf("co gau 1 \n");
	    duc_display(nod);
	  }
	  else printf("ham check !=0 \n");
	  
	  if(sendto(sockfd,msg1,sizeof(msg1),0,(struct sockaddr*)&clie_addr,sizeof(clie_addr)) < 0) {
	error("Khong gui dc");
      }
    }
    else 
    {
      if (strstr(&msg_rec[0][0],"RESPONSE")) {			// nhan lai ban tin RESPONSE xu li luu ip  
      fprintf(stdout,"%s\n",clock_microsecond());
      fprintf(stdout,"%s From: \nIP: %s\n\n",&msg_rec[0][0],inet_ntoa(clie_addr.sin_addr));
  
	  if(duc_check(inet_ntoa(clie_addr.sin_addr),nod)==0) { 
	   printf("ghi vao links :%s",inet_ntoa(clie_addr.sin_addr));
	   duc_add(&msg_rec[0][8],inet_ntoa(clie_addr.sin_addr));
	   printf("co gau 2\n");
	   duc_display(nod);
	  }
	  else printf("ham check !=0 \n");
      }
    else 
    {
      if(msg_rec[0][0] ==64)
      {
	//fprintf(stdout,"nhan ban tin vao links %s\n",&msg_rec[0][0]);
	//fprintf(stdout,"nhan ban tin vao links %s\n",&msg_rec[1][0]);
	
	/*test nod*/
	printf("*********************Test nod***********************\n");
	printf("Gia tri dau tien cua mang: %s\n",&msg_rec[0][0]);
	
	for(i=0;i<32;i++){
	  duc_links_arr(&msg_rec[i][0]);
	}
	printf("*********************88888888***********************\n");
      }
      else	// viet neu thich
      {	
	  fprintf(stdout,"khong ho tro ban tin nay:%s \n!",msg_rec[0]);
	  fprintf(stdout,"khong ho tro ban tin nay:%s \n!",msg_rec[1]);
      }
    }
  }
  }
  fprintf(stdout,"Waiting done... Byebye !\n");
  pthread_exit(NULL);
}
/*Ham phuc vu thread killing*/
  void *killing_handle(void *t){
  char q[5];
  while(!out){
    fprintf(stdout,"Enter q to quit:\n");
    fscanf(stdin,"%s",q);
    if(strstr(q,"q")) {
      out = 1;
      fprintf(stdout,"Tin hieu kill\n");
    }
    else 
      if(strstr(q,"a")){
	duc_display(nod);
 	printf("Size of the list is %d\n",count_link());}
	else
	  if(strstr(q,"b")){
	  printf("vua nhap b \n");
	  soc_ip(nod);
	  broadcast_request_all_interface(1);
	  //send_broadcast("eth0",&msg_ip[0][0]);
	  //memset(msg_ip,'0',1024);
	  //free(msg_ip);
	  }
	  else
	  fprintf(stdout,"Khong phai tin hieu kill\n");
  }
  fprintf(stdout,"Killing done... Byebye !\n");
  pthread_cancel(waiting_thread);
  pthread_exit(NULL);
}
/* ham phuc vu viec updateip_thread*/
 void *dc_update_ip(void *t){
  dc_catch_signal(14, dc_updateip_handler);
  alarm(5);
  pthread_exit(NULL);
}
/*Ham broadcast all interface*/
void broadcast_request_all_interface(int k) {
  /*
   * Set up ban tin request broadcast >> all interface
   */
  //k=0;
  
  if (getifaddrs(&ifaddr) == -1) {
    error("Cant get interface");
  }
  
  for (ifa = ifaddr,n=1; ifa != NULL; ifa = ifa->ifa_next,n++) {
    if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_INET || strstr(ifa->ifa_name,"lo") )
    { n--;continue;}
    
    //if(strstr(ifa->ifa_name,""))
    
    if(getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host,NI_MAXHOST,NULL,0,NI_NUMERICHOST)<0) {
      error("cant get host");
    }
    if(getnameinfo(ifa->ifa_ifu.ifu_broadaddr,sizeof(struct sockaddr_in),broad,NI_MAXHOST,NULL,0,NI_NUMERICHOST)<0) {
      error("cant get broadcast");
    }
    fprintf(stdout,"\nBroadcast interface: %s\n",ifa->ifa_name);
    printf("giao dien mang so %d \n",n);
    fprintf(stdout,"IP: %s \n",host);
    fprintf(stdout,"Broadcast: %s\n",broad);
    if(duc_check(host,nod)==0) {
    duc_add(msg_name,host);
    }
    else printf("ham check !=0 \n");
    
    if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0){
      error("Khong tao dc socket");
    }
    
    if (setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast))<0){
      error("Khong broadcast dc");
    }
    
    memset(&serv_addr,'0',sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
   
    
    if(inet_pton(AF_INET,broad,&serv_addr.sin_addr)<=0) {
      error("Khong convert dc IP broadcast");
    }
    
    switch(k)
    {
      case 0:
    {
   if(sendto(sockfd,msg2,sizeof(msg2),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) {
      error("Khong sendto dc");
    }
    else printf("gui ban tin request %d \n",n);
    //     
    }; break; 
      case 1:							// truyen ip
    {
         if(sendto(sockfd,&msg_ip[0][0],1024,0,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) {
	  error("Khong sendto dc\n");
    }	  else fprintf(stdout,"gui lai ban tin broadcast\n");
    }; break;
      case 2:							// NAME
    {
          if(sendto(sockfd,&msg_name,8,0,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) {
	  error("Khong sendto dc ban tin NAME\n");
	  }	  
	  else fprintf(stdout,"ban tin broadcast gui NAME not mang\n");
    } ; break;

      default : break;
      
    }
    close(sockfd); 
  if(n>=0){			// kiem tra nut co phai la gw hay khong 
    gw =1 ;			// neu la gw thi servic phai co chuc nang truyen ip sang
    printf("La Getway %d\n\n",n); 
  } else printf("No Getway %d\n\n",n);
}

}
/*send broadcast interface selection */
void send_broadcast(char *bcat,char *s) {
  
   if (getifaddrs(&ifaddr) == -1) {
    error("Cant get interface");
  }
  
  for (ifa = ifaddr,n=1; ifa != NULL; ifa = ifa->ifa_next,n++) {
    if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_INET || strstr(ifa->ifa_name,"lo") )
    { n--;continue;}
    
    
    if(getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host,NI_MAXHOST,NULL,0,NI_NUMERICHOST)<0) {
      error("cant get host");
    }
    if(getnameinfo(ifa->ifa_ifu.ifu_broadaddr,sizeof(struct sockaddr_in),broad,NI_MAXHOST,NULL,0,NI_NUMERICHOST)<0) {
      error("cant get broadcast");
    }
    fprintf(stdout,"\nBroadcast interface: %s\n",ifa->ifa_name);
    printf("giao dien mang so %d \n",n);
    fprintf(stdout,"IP: %s \n",host);
    fprintf(stdout,"Broadcast: %s\n",broad);
    duc_add(msg_name,host);
    if((sockfd = socket(AF_INET,SOCK_DGRAM,0)) < 0){
      error("Khong tao dc socket");
    }
    
    if (setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof(broadcast))<0){
      error("Khong broadcast dc");
    }
    
    memset(&serv_addr,'0',sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
   
    
    if(inet_pton(AF_INET,broad,&serv_addr.sin_addr)<=0) {
      error("Khong convert dc IP broadcast");
    }
    
    //if(strstr(ifa->name,bcat))
    //{
    //  goto send_bcat;
    //}
    close(sockfd); 
}
//
}
 
void duc_add(char *name,char *ip){
    printf("bat dau ghi vao links add \n");
    struct node *temp=malloc(sizeof(struct node));
    temp->name=strdup(name);
    temp->ip = strdup(ip);
    if (hear== NULL)
    {
    hear=temp;
    hear->next=NULL;
    }
    else
    {
    temp->next=hear;
    hear=temp;
    }
  
}

void duc_delete_all(){
    struct node *temp, *prev;
    temp=hear;
    printf("nhay den ham delete all \n");
    while(temp!=NULL)
    {
        if(temp==hear)
        {
        hear=temp->next;
        }
        else
        {
        prev->next=temp->next;
	exit(1);
        }
                prev=temp;
        temp= temp->next;
    }
}
 
int duc_check(char *ip1,struct node *rc){	
    char * lo;
    lo = ip1;
    rc= hear;
    printf("nhay den ham check\n");
    if(rc==NULL)
    {
    return 0;
    }
    while(rc!=NULL)
    {
    if(strstr(lo,rc->ip)!=0) {
      printf("ip %s trung voi rc->ip %s",lo,rc->ip);
      return 1;}
      else rc=rc->next;
    }
  return 0;
}
  
void  duc_display(struct node *r){
    
    r=hear;
    if(r==NULL)
    {
    return ;
    }
    else
    while(r!=NULL)
    {
    printf("Name :%s ",r->name);
    printf("IP   :%s \n",r->ip);
    r=r->next;
    }
}

int count_link(){
    struct node *n;
    int c=0;
    n=hear;
    while(n!=NULL)
    {
    n=n->next;
    c++;
    }
    return c;
}
 
void error(char *msg) {
  fprintf(stderr,"%s: %s\n",msg,strerror(errno));
  exit(1);
}

int dc_catch_signal(int sig, void (*handler)(int)){
  struct sigaction action;
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  return sigaction (sig, &action, NULL);
}

void dc_updateip_handler(int sig){
  printf("Ip da duoc update\n");
  soc_ip(nod);
  broadcast_request_all_interface(1);
  alarm(5);
}
char* now() {
  time_t t;
  time (&t);
  return asctime(localtime (&t));
}

void name(char *file_name){

  int i;
  FILE *fd = fopen(file_name,"r");
   if (fd == NULL) {
    error("cant open file");}
      fgets(msg_name,8,fd);
        for(i=0;i<32;i++){
      if(msg_name[i] =='\n')msg_name[i] = '\0';}
	fprintf(stdout,"ten thiet bi :%s \n",msg_name);      
      sprintf(msg1,"%s%s",MSG1,msg_name);
      sprintf(msg2,"%s%s",MSG2,msg_name);
      printf("test ban tin msg1 :%s \n",&msg1[0]);
      printf("tets ban tin msg2 :%s \n",&msg2[0]);
    fclose(fd);  
}

void soc_ip(struct node *r){
    int j = 0;
    r=hear;
    if(r==NULL)
    {
    return;
    }
    while(r!=NULL)
    {
    sprintf(&msg_ip[j][0],"@%s:%s",r->name,r->ip);
    fprintf(stdout,"msg thu duc :%s\n",&msg_ip[j][0]);
    r=r->next;
    j++;
    }
} 

const char* clock_microsecond() {
  static char buf[4][sizeof("00:00:00.000000")];
  static int idx = 0;
  char *ret;
  struct timeval now;
  int sec, usec;

  ret = buf[idx];
  idx = (idx+1) & 3;

  gettimeofday(&now, NULL);

  sec = (int)now.tv_sec + get_timezone();
  usec = (int)now.tv_usec;

  snprintf(ret, sizeof(buf), "%02u:%02u:%02u.%06u",
           (sec % 86400) / 3600, (sec % 3600) / 60, sec % 60, usec);

  return ret;
}

static int get_timezone(void){
#define TIMEZONE_UNINITIALIZED -1

  static int time_diff = TIMEZONE_UNINITIALIZED;
  int dir;
  struct tm *gmt, *loc;
  struct tm sgmt;
  time_t t;

  if (time_diff != TIMEZONE_UNINITIALIZED) {
    return time_diff;
  }

  t = time(NULL);
  gmt = &sgmt;
  *gmt = *gmtime(&t);
  loc = localtime(&t);

  time_diff = (loc->tm_hour - gmt->tm_hour) * 60 * 60
    + (loc->tm_min - gmt->tm_min) * 60;

  /*
   * If the year or julian day is different, we span 00:00 GMT
   * and must add or subtract a day. Check the year first to
   * avoid problems when the julian day wraps.
   */
  dir = loc->tm_year - gmt->tm_year;
  if (!dir) {
    dir = loc->tm_yday - gmt->tm_yday;
  }

  time_diff += dir * 24 * 60 * 60;

  return (time_diff);
}

int get_file_size(FILE *fd) {
  fseek(fd,0,SEEK_END);
  int size = ftell(fd);
  fseek(fd,0,SEEK_SET);
  return size;
}

void duc_links_arr(char *line){		// ghi loc thong tin cua mang de ghi vao 
  
  char *name,*sip_add;
  int i=0,k=0;
  //printf("nhay den ham chuyen ip vao linker list \n");
  //printf("Gia tri line nhan vao:%s\n",line);
  int temp = strlen(line);
  if(temp>10){
   for(i=0;i<32;i++) {
    if(line[i] ==':')k=i;}
  
  sip_add = line+k+1;
  *(line+k) = '\0';
  name = line+1;
     printf("ghi name vao links : %s \n",name);
     printf("ghi ip vao links %s \n",sip_add);
  if(duc_check(sip_add,nod)==0) {
    duc_add(name,sip_add);
    duc_display(nod);
    name=NULL;
    sip_add=NULL;
  }
  else printf("ham check !=0 \n"); 
  }
}

int main(int argc,char *argv){  
  name("name.txt");
  /*Create thread*/
  if(pthread_create(&waiting_thread,NULL,waiting_handle,NULL)<0){
    error("Cant create thread waiting\n");
    }else printf("create Waiting\n");
  if(pthread_create(&killing_thread,NULL,killing_handle,NULL)<0){
    error("Cant create thread kill\n");
    }else printf("create killing\n");
  if(pthread_create(&updateip_thread,NULL,dc_update_ip,NULL)<0){
    error("Cant create thread update ip\n");
    }else printf("create update ip\n");
  
    /*Broadcast all interface*/
  broadcast_request_all_interface(0);

  /*Join thread*/
  void *result;
  
  if(pthread_join(waiting_thread,&result)<0) 	error("cant join thread waiting");
    else printf("join wait\n");
  if(pthread_join(killing_thread,&result)<0) 	error("cant join thread killing");
    else printf("join thread\n");
  if(pthread_join(updateip_thread,&result)<0) 	error("cant join thread update ip");
    else printf("join update ip\n");    
  fprintf(stdout,"Out service\n");
  pthread_exit(NULL);
}
