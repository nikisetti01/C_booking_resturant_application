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
#define PIATTO_DIM 50
#define NUM_PIATTI 8
#define MAX_COMANDA 30
#define DIM_COMF_COMANDA 17
typedef struct{
  char codice[2];
 
  int prezzo;
 char nome[PIATTO_DIM];

}piatto ;
typedef struct{
piatto piatti_in_corso[MAX_COMANDA];
int quanti[MAX_COMANDA];
int status;
int n_ordini;
int numerocomanda;
int codice_tavolo;
} comanda;


int main(int argc, char* argv[]){
  comanda c[MAX_COMANDA];
 


char bufferk[BUFFER_SIZE];
  uint8_t lmsgk;
    int ret, sd,fdmax,i;

int count_comande=0;
fd_set read_fds;
fd_set master;

    struct sockaddr_in srv_addr;
    char buffer[BUFFER_SIZE];
    char cmd[10];
uint8_t lmsg;
    char type='K';



      sd=socket(AF_INET,SOCK_STREAM,0);
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
srv_addr.sin_port =htons(4242/*atoi(argv[1])*/);
inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr);


  ret=connect(sd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
  
 char buftype[2];
  buftype[0]=type;
  
  ret=send(sd, (void*)buftype,2,0);


if(ret<0){
  perror("Richiesta problematica");
  exit(0);

}

    printf("• take --> accetta una comanda \n"); 
    printf("• show --> mostra le comande accettate (in preparazione) \n");
    printf("• ready-->imposta lo stato della comanda (in servizio) \n") ;
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
   scanf("%s",cmd);
   lmsg=strlen(cmd)+1;
     ret=send(sd, (void*)&lmsg,sizeof(uint8_t),0);
   ret=send(sd,(void*)cmd,lmsg,0); 
       



if(strcmp(cmd,"take")==0){

  memset(buffer,0,sizeof(buffer));
  uint8_t ricevo_tavolo;
  ret=recv(sd,(void*)&ricevo_tavolo,sizeof(uint8_t),0);
  c[count_comande].codice_tavolo=ricevo_tavolo;

  ret=recv(sd,(void*)&lmsgk,sizeof(uint8_t),0);
  ret=recv(sd,(void*)buffer,lmsgk,0);
  
  c[count_comande].numerocomanda=buffer[0]-48;


     int counter=0;
                int cursor=3;
              
                while(buffer[counter]!='\0'){
                  c[count_comande].piatti_in_corso[counter].codice[0]=buffer[cursor];
                  cursor++;
                          
                  c[count_comande].piatti_in_corso[counter].codice[1]=buffer[cursor];
                  cursor+=2;
                  c[count_comande].quanti[counter]=buffer[cursor]-48;
                  counter++;
                  cursor+=3;
                }
    c[count_comande].status=1;

   c[count_comande].n_ordini=(counter-3)/5;
                int l;
            int app=c[count_comande].numerocomanda+1;
               printf("comanda  %d  T%d: \n",app ,c[count_comande].codice_tavolo);
            
                for(l=0; l<c[count_comande].n_ordini;l++){
                printf("%s %d\n", c[count_comande].piatti_in_corso[l].codice,c[count_comande].quanti[l] );
                } 
count_comande++;


}
if(strcmp(cmd,"show")==0){

  int ii; 
  int jj;
 for(ii=0; ii<count_comande; ii++){
  if(c[ii].status==1){

  printf("comanda  %d T%d: \n", c[ii].numerocomanda+1,c[ii].codice_tavolo);
  for(jj=0; jj<c[ii].n_ordini;jj++)
  printf("codice:%s quantità:%d\n", c[ii].piatti_in_corso[jj].codice,c[ii].quanti[jj] );




 }
 }


}
if(strcmp(cmd,"ready")==0){
char com[4];
int in_servizio;
int tavolo;
uint8_t mando_tavolo;


scanf("%s T%d",com, &tavolo);
in_servizio=com[3]-48;


int jj;
char bufktemp[BUFFER_SIZE];
in_servizio--;
sprintf(bufktemp, "%d \n", in_servizio);
int ii=0;

for(ii=0; ii<count_comande;ii++){

   if(c[ii].numerocomanda==in_servizio){
 for(jj=0; jj<c[ii].n_ordini; jj++){
 
        
sprintf(bufferk,"%s %d \n",c[ii].piatti_in_corso[jj].codice,c[ii].quanti[jj]);
}
                 
                  strcat(bufktemp, bufferk);
 }
}
 strcpy(bufferk,bufktemp);
printf("%s ", bufferk);
 lmsg=strlen(bufferk)+1;


mando_tavolo=tavolo;
c[in_servizio].status=2;
ret=send(sd,(void*)&lmsg, sizeof(uint8_t),0);
ret=send(sd,(void*)bufferk,lmsg,0);
ret=send(sd,(void*)&mando_tavolo, sizeof(uint8_t),0);

}
              
                }
                else {
                    ret=recv(sd,(void*)&lmsgk,sizeof(uint8_t),0);
  ret=recv(sd,(void*)buffer,lmsgk,0);
     if(strcmp(buffer,"stop")==0){
      printf("Chiusura di tutti i device \n");
      close(sd);
         FD_CLR(sd, &master);
         FD_CLR(i,&master);
         exit(0);
    }
  printf(" %s \n", buffer);

 

                }
            }
        }
    }
  if(ret<0){
    perror("Problemi di connessione");
    exit(0);
  }


sleep(1);


}
