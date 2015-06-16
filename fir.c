#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define file1 		"logbatctl"
#define file2		"nc_code-logbatctl.txt"
#define file3		"nc_decode-logbatctl.txt"

#define funs		"if(dc_debug =1) printk(\" anhduc \");"

int main()
{
   FILE *fp1,*fp2,*fp3;
   char str[1024];
   char str1[1024];
   char str2[2048];
   char ch;
   int write=0;
   /* opening file for reading */
   fp1 = fopen(file1, "a+");
   fp2 = fopen(file2, "w");
   fp3 = fopen(file3, "w");
   if(fp1 == NULL) {
      perror("Error opening file");
      return(-1);
   }
   while(fgets (str, 1024, fp1)!=NULL)
    {
      if(strstr(str,"nc_code:")){
	//write=1;
	//printf("Tim thay void\n");
	fprintf(fp2,"%s",str);
	fprintf(stdout,"nc_recode_byty:duc");
	fprintf(stdout,"%s",str);
      }
       if(strstr(str,"nc_decode:")){
	//write=1;
	//printf("Tim thay void\n");
	fprintf(fp3,"%s",str);
	fprintf(stdout,"nc_decode:duc");
	fprintf(stdout,"%s",str);
      }
      
      //else
      //{
	//fprintf(fp2,"%s",str);
      //}
      
      //if(strstr(str,";")&&write){
	//printf("tim thay cau lenh dau tien\n");
	//sprintf(str1,"%s",funs);
	//sprintf(str2,"%s%s\n",str,str1);
	//fprintf(stdout,"%s",str2);
	//fprintf(fp2,"%s",str2);
	//write=0;
      //}
      
    }
   fclose(fp1);
   fclose(fp2);
   return(0);
  
}