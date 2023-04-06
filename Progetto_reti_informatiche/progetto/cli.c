#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define BUFFER_SIZE 1024
#define DIM_COMFERMA 24
#define DIM_CODE 3
#define DATA_DIM 10
#define DIM_STOP 5
 
void options(){
  printf(".find--> ricerca la disponibilita' per una prenotazione \n ");
  printf(".book--> invia una prenotazione \n");
  printf(".esc --> termina il client \n");
}
// struttura prenotazione con i dati richiesti dal comando find 
typedef struct{
  char cognome[30];
  int dim_cognome;
  int numero;
  char data[10];
int ora;
char codiceprenotazione[3]; 
 char timestamp[9];


} prenotazione; 
int main(int argc, char* argv[]){

  uint16_t lmsg;
    int ret,i, sd, len, fdmax;
fd_set read_fds;
fd_set master;
    prenotazione p;
    struct sockaddr_in srv_addr;
    char buffer[BUFFER_SIZE];
    char cmd[10];
    char type='C';
    char code[3];

      options();
  
   sd=socket(AF_INET,SOCK_STREAM,0);
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;

srv_addr.sin_port = htons(4242/*atoi(argv[1])*/);

inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr);
  ret=connect(sd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
     
 char buftype[2];
  buftype[0]=type;
  // mando il tipo del device 
  ret=send(sd, (void*)buftype,2,0);

  if(ret<0){
    perror("Problemi di connessione");
    exit(0);
  }
  FD_SET(sd,&master);
    FD_SET(0,&master);
    fdmax=sd;
   while(1){
       printf("selezionare comando \n");
   
        read_fds=master;
        ret=select(fdmax+1,&read_fds,NULL,NULL,NULL);
        if(ret<0){
            perror("ERRORE SELECT DEL KITCHEN");
            exit(1);
        }

  for(i=0; i<=fdmax; i++){
            if(FD_ISSET(i,&read_fds)){
               if(i==0){

if(ret<0){
  perror("Richiesta problematica");
  exit(0);

}

printf("Inserire il comando\n");
scanf("%s",cmd);
      len=strlen(cmd)+1;
lmsg=htons(len);
// mando il comando selezionato 
  ret=send(sd, (void*)&lmsg,sizeof(uint16_t),0);
   ret=send(sd,(void*)cmd,len,0);
    
 if(strcmp(cmd, "find")==0){
 
    scanf("%s %d %s %d",p.cognome, &p.numero,p.data, &p.ora ) ;
   sprintf(buffer,"%s %d %s %d",p.cognome,p.numero,p.data,p.ora);
   len=strlen(buffer)+1;
   lmsg= htons(len);
   // mando i dati per la prenotazione 
  ret=send(sd, (void*)&lmsg,sizeof(uint16_t),0);
   ret=send(sd,(void*)buffer,len,0); 

  
   
   
  
  if(ret<0){
    perror("Errore in fase di ricezione: \n");
    exit(-1);
  }
       memset(buffer,0,BUFFER_SIZE);
       // ricevo le opzioni 
                   ret=recv(sd, (void*)&lmsg, sizeof(uint16_t),0);
                     
                      len=ntohs(lmsg); 
                               
             ret=recv(sd,(void*)buffer,len,0); 
             if(ret<0){
              perror("Errore ricezione tavoli");
              exit(0);
             }
             printf("buffer non funge %s \n", buffer);
           

 }
 if(strcmp(cmd,"book")==0){
  len=strlen(cmd)+1;
lmsg=htons(len);


  printf("sono nella book \n");

char prova[2];
int prova2=0;
  scanf("%d", &prova2);
  sprintf(prova, "%d",prova2);
 len = strlen(prova) + 1;

lmsg = htons(len);

// mando la scelta 
ret = send(sd, (void*) &lmsg, sizeof(uint16_t), 0);

ret = send(sd, (void*) prova, len, 0);

 
   printf("%s \n", prova);
  if(ret<0){
    printf("problema invio book");
    exit(0);
  }
  len=DIM_COMFERMA;
  memset(buffer,0,BUFFER_SIZE);
  // ricevo comferma prenotaione 
  ret=recv(sd,(void*)buffer,DIM_COMFERMA,0);
  printf("%s \n", buffer);
    if(ret<0){
    perror("problema di conferma");
    exit(0);
  }

  buffer[DIM_COMFERMA]='\0';
  // ricevo il codice che dovrò utilizzare all'avvio del table
  ret=recv(sd,(void*)code,DIM_CODE,0);
  
  printf("il tuo codice : %s \n", code);
  


  
 }

  if(strcmp(cmd,"esc")==0){
      printf("Arrivederci \n");
       close(sd);
         FD_CLR(sd, &master);
         FD_CLR(i,&master);
         exit(0);
 
    }


 
}

  else{ 
// messaggio di stop per chiudere il device 
    memset(buffer,0,strlen(buffer));
    ret=recv(sd,(void*)&lmsg,sizeof(uint8_t),0);
    ret=recv(sd,(void*)buffer,lmsg,0);
   
    if(strcmp(buffer,"stop")==0){
      printf("Chiusura di tutti i device \n");
      close(sd);
         FD_CLR(sd, &master);
         FD_CLR(i,&master);
         exit(0);
    }
 }



  
}
  }
   }
}

