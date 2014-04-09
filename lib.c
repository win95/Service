#include "lib.h"

void error(char *msg) {
  fprintf(stderr,"%s: %s\n",msg,strerror(errno));
  exit(1);
}

char* now() {
  time_t t;
  time (&t);
  return asctime(localtime (&t));
}

void write_file(char *file_name,char *msg){
  FILE *fd = fopen(file_name,"a");
  if (fd == NULL) {
    error("cant open file");    
  }
  fprintf(fd,"%s\n",msg);
  fclose(fd);
}

void write_file1(char *file_name,char *msg){
  FILE *fd = fopen(file_name,"a");
  if (fd == NULL) {
    error("cant open file");    
  }
  fprintf(fd,":%s\n",msg);
  fclose(fd);
}
void write_file2(char *file_name,char *msg1,char *msg2){
  FILE *fd = fopen(file_name,"a");
  if (fd == NULL) {
    error("cant open file");    
  }
  fprintf(fd,":%s%s",msg2,msg1);
  fclose(fd);
}

void name(char *file_name){

  FILE *fd = fopen(file_name,"r");
   if (fd == NULL) {
    error("cant open file");}
      while(fgets(msg_name,8,fd)!=NULL)
      {
	fprintf(stdout,"ten thiet bi :%s \n",msg_name);
      }
      sprintf(msg1,"%s@%s",MSG1,msg_name);
      sprintf(msg2,"%s@%s",MSG2,msg_name);
      printf("test ban tin msg1 :%s \n",&msg1[0]);
      printf("tets ban tin msg2 :%s \n",&msg2[0]);
      //printf("phan tu msg2 : %s \n",msg2);
      //printf("phan tu msg1 : %s \n",&msg1[24]);
      //printf("phan tu msg2 : %s \n",&msg2[24]);
    fclose(fd);  
}


int check_ip(char *file_name,char *ip){
  FILE *fd = fopen(file_name,"r");
  char buf[100];
  if (fd == NULL) {
    error("cant open file");    
  }
  while(fgets(buf,100,fd)!=NULL) {
    if (strstr(buf,ip)) {
	fclose(fd);
	return 1;
	}
  }
  fclose(fd);
  //fclose(file_name);  //sua
  return 0;
}



void soc_ip(){

  FILE *fd = fopen("ip_table.txt","r");
  int i=0,j=0;
  if(fd == NULL) error("can't open file");
      
	while(fgets(&msg_ip[j][0],32,fd)!=NULL)
	{
	  j++;
	} 
  fclose(fd);
}



const char* clock_microsecond() {    // gtt
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

/**
 * Returns the difference between gmt and local time in seconds.
 * Use gmtime() and localtime() to keep things simple.
 * 
 * taken and slightly modified from www.tcpdump.org.
 */
static int get_timezone(void)
{
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

