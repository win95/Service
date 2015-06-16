#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main()
{
  fprintf(stdout,"Tool debug batman\n");
  system("sudo rm logbatctl");
  while(1)
  {
    
    system("sudo batctl s >> logbatctl");
    usleep(1000000);
  }
}