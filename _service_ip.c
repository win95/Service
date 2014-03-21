#include "lib.h"

int out = 0;
char q[10];
int 	gw;
char host[NI_MAXHOST];
  struct ifaddrs *ifaddr, *ifa;
  int n;
  char *hot = "192.160.0.0";
  char broad[NI_MAXHOST];
  int broadcast = 1;
  char msg[MAXMSG];
pthread_t waiting_thread;
pthread_t killing_thread;
pthread_t listen_thread;
struct sockaddr_in serv_addr;
struct sockaddr_in addr_remote;
int sockfd = 0;
FILE *fd;
int sin_size; // to store struct size
    int nsockfd; // New Socket file descriptor
    int num;
/*Ham phuc vu thread waiting*/
void *waiting_handle(void *t){
  int sockfd = 0;
  char msg[MAXMSG];
  size_t size;
  struct sockaddr_in serv_addr;
  struct sockaddr_in clie_addr;
  
  if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0) {
    error("Khong tao dc socket");
  }
  
  memset(&serv_addr, '0', sizeof(serv_addr));
  memset(&clie_addr, '0', sizeof(clie_addr));
  memset(msg,'0',sizeof(msg));
  
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
    if(recvfrom(sockfd,msg,MAXMSG,0,(struct sockaddr*)&clie_addr,&size)<0) {
      error("Khong nhan dc ban tin");
    }
    
    //fprintf(stdout,"Got msg: %s\n",msg);
    
    if (strstr(msg,"REQUEST")) {
      fprintf(stdout,"%s\n",clock_microsecond());
      fprintf(stdout,"%s From: \nIP: %s\n\n",msg,inet_ntoa(clie_addr.sin_addr));
      clie_addr.sin_port = htons(PORT);
      /*
       * Luu lai IP o day
       */
      //if(check_ip("ip_table.txt",inet_ntoa(clie_addr.sin_addr))==0)
      //write_file("ip_table.txt",inet_ntoa(clie_addr.sin_addr));
      if(sendto(sockfd,MSG1,sizeof(MSG1),0,(struct sockaddr*)&clie_addr,sizeof(clie_addr)) < 0) {
	error("Khong gui dc");
      }
    }
    else if (strstr(msg,"RESPONSE")) {			// nhan lai ban tin RESPONSE xu li luu ip
      
    //fprintf(stdout,"\nRESPONSE\nFrom \nIP: %s\nPort: %i\n",inet_ntoa(clie_addr.sin_addr),clie_addr.sin_port);  
      fprintf(stdout,"%s\n",clock_microsecond());
      fprintf(stdout,"%s From: \nIP: %s\n\n",msg,inet_ntoa(clie_addr.sin_addr));
      if(check_ip("ip_table.txt",inet_ntoa(clie_addr.sin_addr))==0)
	if(gw ==1) write_file("ip_table.txt",inet_ntoa(clie_addr.sin_addr));
      
    }
    else {
      fprintf(stdout,"Khong ho tro ban tin! \n");
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
    else fprintf(stdout,"Khong phai tin hieu kill\n");
  }
  fprintf(stdout,"Killing done... Byebye !\n");
  pthread_cancel(waiting_thread);
  pthread_exit(NULL);
}

/*Ham broadcast all interface*/
void broadcast_request_all_interface() {
  /*
   * Set up ban tin request broadcast >> all interface
   */

  
  if (getifaddrs(&ifaddr) == -1) {
    error("Cant get interface");
  }
  
  for (ifa = ifaddr,n=0; ifa != NULL; ifa = ifa->ifa_next,n++) {
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
    write_file("ip_table.txt",host);
    fprintf(stdout,"Broadcast: %s\n",broad);
    
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
    
    if(sendto(sockfd,MSG2,sizeof(MSG2),0,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) {
      error("Khong sendto dc");
    }
    else printf("gui ban tin request %d \n",n);
    //close(sockfd);      
  } 
  if(n>=2){			// kiem tra nut co phai la gw hay khong 
    gw =1 ;			// neu la gw thi servic phai co chuc nang truyen ip sang
    printf("La Getway %d\n\n",n); 
  } else printf("No Getway %d\n\n",n);
}

void *listen_handle(void *t){
  int *x;
  int y;
  x=&y;
  pthread_cancel(listen_thread);
  pthread_exit(NULL);
}

int main(int argc,char *argv){
  
  /*Clear file*/
  fd = fopen("ip_table.txt","w");
  fclose(fd);
  
  /*Create thread*/
    if(pthread_create(&listen_thread,NULL,listen_handle,NULL)<0){
    error("Cant create ");
    }else printf("create listing\n");
  if(pthread_create(&waiting_thread,NULL,waiting_handle,NULL)<0){
    error("Cant create thread waiting\n");
    }else printf("create Waiting\n");
  if(pthread_create(&killing_thread,NULL,killing_handle,NULL)<0){
    error("Cant create thread kill\n");
    }else printf("create killing\n");
  
  /*
  if(pthread_create(listen_c(PORT),NULL,listen_c(PORT),NULL)<0){
    error("Cant create thread kill");
  }
  */
  /*Broadcast all interface*/
  broadcast_request_all_interface();
  
  if(gw==0)
  {
  fd = fopen("ip_table.txt","w");
  fclose(fd);
  }
    
  /*Join thread*/
  void *result;
  
  if(pthread_join(listen_thread,&result)<0) 	error("cant join listen_c");
    else printf("join listen");
  if(pthread_join(waiting_thread,&result)<0) 	error("cant join thread waiting");
    else printf("join wait");
  if(pthread_join(killing_thread,&result)<0) 	error("cant join thread killing");
    else printf("join thread");
  fprintf(stdout,"Out service\n");
  pthread_exit(NULL);
}