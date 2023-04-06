#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define MAX_DEVICE 10
// struttura che ientifica un device 
typedef struct {
int sd;// socket
char tp; // tipo
} device;
// scrivo nel file connessioni.txt il device nel formato SD TIPO 
// richiedo in input la struttura device e il numero di device  
void carica_device(device*d, int numdevice){



  int i;
 FILE*fptr;
    fptr=fopen("connessioni.txt","w");
    if(fptr == NULL)
   {
      printf("Error!");   
      exit(1);             
   }
  for(i=0; i<numdevice; i++){
    fprintf(fptr,"%d %c \n", d[i].sd, d[i].tp);
  
  }
  fclose(fptr);
}
void trova_socket(char types, int*type_sd, int numdevice,int* count){
  FILE* fptr;
  if((fptr = fopen("connessioni.txt","r")) == NULL){
       printf("Error! opening file");

       // Program exits if the file pointer returns NULL.
       exit(1);
   }
  int i;
  int socket;
  char type;
 *count=0;

   for(i=0;i<numdevice;i++ ){
     fscanf(fptr,"%d %c",&socket,&type );
   
    if(type==types){
    type_sd[*count]=socket;

      (*count)++;
      
     } 



   }
}

   // leggo il tipo del socket messo in input se esiste, altrimenti restituisco 0-> socket non presente nel file connessioni.txt
  // richieto in input il nuovo socket se non esiste nel file restituisco 0 altrimenti restituisco il tipo
int  leggi_tipo(int newsocket, char* newtype, int numdevice){
   FILE* fptr;
   int socket;
   char type;
   int controllo=0;
   int i;
  
   if((fptr = fopen("connessioni.txt","r")) == NULL){
       printf("Error! opening file \n");

       // Program exits if the file pointer returns NULL.
       exit(1);
   }
    for( i=0; i<numdevice; i++){
   
      fscanf(fptr,"%d %c",&socket,&type );
    
  
      if(socket==newsocket){
        *newtype=type;
        controllo=1;
      }
      

    }
    return controllo;
fclose(fptr);
}
