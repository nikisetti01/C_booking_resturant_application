#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "header.h"
#define REQ_LEN 5
#define MAX_COMANDA 30
#define BUFFER_SIZE 1024
#define PIATTO_DIM 50
#define NUM_PIATTI 30
// inizializzo le strutture utili per il server 
// struttura che identifica il piatto ordinabile
typedef struct{
  // codice del piatto
  char codice[2];
 
  int prezzo;
 char nome[PIATTO_DIM];

}piatto ;
// struttura dati che permette di associare ad ogni tavolo il socket corrispondente 
typedef struct{
  int socket;
  int tavolo;

}tavoli_in_servizio;
// struttura comanda con all'intero un array di strutture piatti 
typedef struct{
piatto piatti_in_corso[MAX_COMANDA];
int quanti[MAX_COMANDA];
int n_ordini;
int status;
int codice_tavolo;
} comanda;
// funzione che dato in input un socket restituisce il tavolo a esso associato 
int trova_tavolo(int sock,tavoli_in_servizio*t){
  int i; 
  int tavolo_target;
  // 3 sono i table che facciamo partire con lo script 
  for(i=0; i<3; i++){
  if(sock== t[i].socket)
  tavolo_target=t[i].tavolo;
  }
  return tavolo_target;
}
// funzione che associa l'intero status (0,1,2) a cosa stampare 
void converti_status(int s){
  switch(s){
    case 0: 
    printf("<in attesa> ");
    break;
    case 1:
    printf("<in preparazione> ");
    break;
    case 2: 
    printf("<in servizio> ");
  }
}
// funzione che dato in input un numero di tavolo  restituisce il socket  a esso associato 
int trova_socket_table(int table,tavoli_in_servizio*t){

   int i; 
  int socket_target;
  for(i=0; i<3; i++){
  if(table== t[i].tavolo)
  socket_target=t[i].socket;
  }
  return socket_target;

}
// salviamo le comande registrate in un file
 void salva_comanda(comanda* c,int count_comande){
 
  int i,j;
   FILE* fptr;
    fptr=fopen("comande.txt","w");
    if(fptr==NULL){
        printf("Error sono io il problema!");
        exit(1);
    }
             
    for(i=0; i<count_comande;i++){
       fprintf(fptr,"COMANDA %d T%d \n",i, c[i].codice_tavolo);
        
        for(j=0; j<c[i].n_ordini;j++){
          fprintf(fptr,"%s %d ", c[i].piatti_in_corso[j].codice,c[i].quanti[j]);
    
    
        }
        fprintf(fptr,"\n");
    }
fclose(fptr);
}


int main(int argc, char*argv[]){
  // inizializzo variabili per le strutture dati o per la connessione mediante I/O multiplexing 
  tavoli_in_servizio t[2];
  int t_sd;

comanda c[MAX_COMANDA];
int count_comande=0;
char type;
char buffert[BUFFER_SIZE];
char buffertemp[BUFFER_SIZE];


uint8_t lmsgt,lmsgk;

char bufferk[BUFFER_SIZE];
int count=0;
prenotazione prenotaizioni[100];
char code[3];
   
   int codice=0;
  giorno_feriale libro_prenotazioni[GIORNIPRENOTABILI];
  int check;
  int k_count=0;
  int t_count=0;
  uint8_t stato;
  int kitchen_sd[MAX_DEVICE];
  int table_sd[MAX_DEVICE];

//inizzializzo il libro delle prenotazioni cone leprenotazioni fino ad oggi
   leggi_libro(libro_prenotazioni);
    tavolo* tavoli_possibili;
    tavoli_possibili=malloc(TAVOLIMAX*sizeof(tavolo));
    int dim;
    int ret,clifd,listener, i, len;
    socklen_t addrlen;

    int in_tavola=0;
    char cmd[10];

    int numdevice=0;
    device d[MAX_DEVICE];
    prenotazione p;
    uint16_t lmsg;


    struct sockaddr_in my_addr,cl_addr;
    fd_set master;
    fd_set read_fds;
    int fdmax;
    char buffer[BUFFER_SIZE];
    listener=socket(AF_INET,SOCK_STREAM,0);
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family=AF_INET;
    my_addr.sin_port=htons(atoi(argv[1]));
    my_addr.sin_addr.s_addr=INADDR_ANY;
    ret=bind(listener,(struct sockaddr*)&my_addr, sizeof(my_addr));

if( ret < 0 ){
perror("Bind non riuscita\n");
exit(0);
}
listen(listener, 10);
FD_ZERO(&master);
FD_ZERO(&read_fds);
FD_SET(listener, &master);
FD_SET(0, &master);

fdmax=listener;
while(1){
    read_fds=master;
    ret=select(fdmax+1, &read_fds,NULL,NULL,NULL);
    if(ret<0){
        perror("ERRORE  SELECT");
    }
    for(i=0; i<=fdmax;i++){
        if(FD_ISSET(i,&read_fds)){
            if(i==listener){
         
                 printf("Nuovo device \n");
                 fflush(stdout);
             
                 
                addrlen=sizeof(cl_addr);
             
                clifd=accept(listener,(struct sockaddr*)&cl_addr, &addrlen);
             
      // ogni volta che arriva un nuovo device esso mi invia anche il suo tipo 
                char buftype[2];
                ret=recv(clifd,(void*)buftype,2,0 );
                sscanf(buftype,"%c",&type);
         
    // guardo se il device è nuovo e se non lo è gli associo il tipo 
                check=leggi_tipo(clifd,&type,numdevice );
                
               if(check==0){
    // altrimenti lo carico sia nell'array che nel file
               d[numdevice].sd=clifd;
            d[numdevice].tp=type;
                  numdevice++;

             carica_device(d,numdevice);

                }
               
             
                FD_SET(clifd, &master);
              
                if(clifd>fdmax){
                   fdmax=clifd;
                }
                   
            }
            // se utilizzo l'input file
          else      if(i==0){
     
                  char indicazione[5];
        
         
                    int jj;
                    int ii;
 
                     scanf("%s",cmd);
                     // se il comando è stat 
                      if(strcmp(cmd,"stat")==0){
         
                                 int t_target;
                      fgets(indicazione,sizeof(indicazione),stdin);
                        //inserisco il tavolo richiesto 
                        sscanf(indicazione," T%d", &t_target);
           
                           for(ii=0; ii<count_comande; ii++){
                              if(indicazione[1]=='a'){
                                     if(c[ii].status==0){
     
                                       printf("com%d T%d \n",ii+1,c[ii].codice_tavolo);
               
                                       for(jj=0; jj<c[ii].n_ordini;jj++){
                                          printf("  %s %d \n", c[ii].piatti_in_corso[jj].codice,c[ii].quanti[jj]);
                  
                                             }
                                           fflush(stdout);
                                }
                              }
                               if(indicazione[1]=='p'){
                                  if(c[ii].status==1){
               
                                        printf("com%d T%d \n",ii+1,c[ii].codice_tavolo);
               
                                                           for(jj=0; jj<c[ii].n_ordini;jj++){
                                                               printf("  %s %d \n", c[ii].piatti_in_corso[jj].codice,c[ii].quanti[jj]);
                  
                                                                                          }
                                                                                fflush(stdout);
                                                                       }
                                                                         }
                               if(indicazione[1]=='s'){
                 
                                         if(c[ii].status==2){
         
                                          printf("com%d T%d \n",ii+1,c[ii].codice_tavolo);
             
                                        for(jj=0; jj<c[ii].n_ordini;jj++){
                                        printf("  %s %d \n", c[ii].piatti_in_corso[jj].codice,c[ii].quanti[jj]);
                  
                                        }
                                          fflush(stdout);
                                                             }
                                                                 }

                                           if(c[ii].codice_tavolo==t_target){
             
                                             printf("com%d ",ii+1);
                                             converti_status(c[ii].status);
                                             for(jj=0; jj<c[ii].n_ordini;jj++){
                                            printf(" \n %s %d \n", c[ii].piatti_in_corso[jj].codice,c[ii].quanti[jj]);
                 
                                            }
                                      fflush(stdout);

              }
                                    else 
                                      if(strlen(indicazione)<=1){
                                        
                                        printf("com%d T%d ",ii+1,c[ii].codice_tavolo);
                                            converti_status(c[ii].status);
                                              for(jj=0; jj<c[ii].n_ordini;jj++){
                                                printf(" \n %s %d \n", c[ii].piatti_in_corso[jj].codice,c[ii].quanti[jj]);
                                            }
                                                fflush(stdout);
                                            }
                                          }

                                        }
                        if(strcmp(cmd,"stop")==0){
                          int chiusura_possibile=1;
                          int ii;
                          int jj;
                            for(ii=0; ii<count_comande; ii++){
                               // controllo se ci sono comande non in servizio
                                  if(c[ii].status !=2)
                                  chiusura_possibile=0;
                                }
                    
                          if(chiusura_possibile==1){
                            int k=0;
                            // altrimenti mando un messaggio di chiusura a tutti i device
                            for(k=0; k<numdevice;k++){
                                  
                                  lmsg=strlen(cmd)+1;
                                ret= send(d[k].sd,(void*)&lmsg,sizeof(uint8_t),0);
                                ret= send(d[k].sd,(void*)cmd,lmsg,0);
                                  }

                                } else
                                printf("chiusura non possibile \n");
                              }
                    }
                    else{ 
                          // leggo il tipo e a secondo eseguo diverse azioni 
                        check=leggi_tipo(i,&type,numdevice);
                       
                     
                        fflush(stdout);
                           if(type=='T'){
            
                            // ricevo il comando 
                 ret=recv(i, (void*)&lmsgt, sizeof(uint8_t),0);
                ret=recv(i, (void*)cmd,lmsgt,0);
                     if(ret==0){

                      
                        printf("CHIUSURA client rilevata! \n");
                        fflush(stdout);
                        close(i);
                        FD_CLR(i, &master);
                    } 
          else if(ret<0){
                        perror("Errore ricezione  msg \n");
                        close(i);
                          FD_CLR(i, &master);
                        
                    } 
            
                if(strcmp(cmd, "comanda")==0){
                
                 memset(buffert,0,BUFFER_SIZE);
            
                    // ricevo il buffer 
                    ret=recv(i, (void*)&lmsgt, sizeof(uint16_t),0);
                
                ret=recv(i, (void*)buffert,lmsgt,0);
            
                if(ret<0){
                  perror("errore ricezione comanda ");
                }
                
               
                   int counter=0;
                int cursor=0;
            // faccio il parsing carattere per carattere della comanda ricevuta dal table
                while(buffert[counter]!='\0'){
                  c[count_comande].piatti_in_corso[counter].codice[0]=buffert[cursor];
                  cursor++;
                          
                  c[count_comande].piatti_in_corso[counter].codice[1]=buffert[cursor];
                  cursor+=2;
                  c[count_comande].quanti[counter]=buffert[cursor]-48;
          
                  counter++;
                  cursor+=2;

                

                  
                }
                
      
               c[count_comande].n_ordini=counter/5;
               // comanda in attesa
              c[count_comande].status=0;
              // trovo il tavolo della comanda dal socket dal quale l'ho ricevuto
              c[count_comande].codice_tavolo=trova_tavolo(i,t);
               
              
           
              salva_comanda(c, count_comande);
              
              
              memset(buffert,0,strlen(buffert));

              strcpy(buffert,"COMANDA RICEVUTA");
              len=strlen(buffert);
              ret=send(i,(void*)buffert,len,0);
              memset(buffert,0,strlen(buffert));
              // rimando la comanda ricevuta al table device 
              sprintf(buffertemp, "com%d <in attesa> \n", count_comande+1);
               
               int jj;
  
 for(jj=0; jj<c[count_comande].n_ordini; jj++){
                   
                  sprintf(buffert,"%s %d \n",c[count_comande].piatti_in_corso[jj].codice,c[count_comande].quanti[jj]);
                 
                  strcat(buffertemp, buffert);
 }
 // incremento il numero di comande nella struttura
    count_comande++;
 strcpy(buffert,buffertemp);
              
               int l;
               lmsg=strlen(buffert)+1;
              ret=send(i,(void*)&lmsg,sizeof(uint8_t),0);
               ret=send(i,(void*)buffert,lmsg,0);
          
   // mando la notficia al kitchen device      
                memset(bufferk,0,strlen(bufferk));
                  char a='*';

                int contatore=0;
                  for(l=0; l<count_comande; l++){
                  
                    if(c[l-1].status==0){
                    
                    bufferk[contatore]=a;
                
                    contatore++;
                    
                    }

                  }
                



                      fflush(stdout);  

                      lmsgk=strlen(bufferk)+1;


                      // trovo tutti i kitchen device e mando a ognuno la notficia 
                      trova_socket('K',kitchen_sd,numdevice,&k_count);
                      fflush(stdout);

                      fflush(stdout);
                      int h;
                      for(h=0; h<k_count;h++){
                      
                      ret=send(kitchen_sd[h],(void*)&lmsgk,sizeof(uint8_t),0);
                      ret=send(kitchen_sd[h],(void*)bufferk,lmsgk,0);
                      }
                                
                
                
               } 
                                  

                    } 

       if(type=='C'){
           
                 ret=recv(i, (void*)&lmsg, sizeof(uint16_t),0);

               
                len=ntohs(lmsg);
             
                ret=recv(i, (void*)buffer,len,0);
                     if(ret==0){

                    
                        printf("CHIUSURA client rilevata! \n");
                        fflush(stdout);
                        close(i);
                        FD_CLR(i, &master);
                    } 
          else if(ret<0){
                        perror("Errore ricezione  msg \n");
                        close(i);
                          FD_CLR(i, &master);
                        
                    } 
               
          
                strcpy(cmd, buffer);
         
        
                        if(strcmp(cmd,"find")==0){
                    
                            
                             // ricevo la dim del cognome 
                   ret=recv(i, (void*)&lmsg, sizeof(uint16_t),0);
                    // la trasformo con dim ninja
                      len=ntohs(lmsg); 
                   
                        // attendo le informazioni della prenotazione                          
             ret=recv(i,(void*)buffer,len,0); 
                       sscanf(buffer,"%s %d %s %d",p.cognome,&p.numero,p.data,&p.ora);
                 
                    // cerco i possibili tavoli prenotbili
                     prenota(libro_prenotazioni,tavoli_possibili,p,&dim);
                    
              char buftemp[BUFFER_SIZE];
                  memset(buffer,0,BUFFER_SIZE);
                  memset(buftemp,0,BUFFER_SIZE);
                     int k;
             
// mando le possibili scelte al client 
   for(k=0; k<dim; k++){
   
   sprintf(buffer,"%d) T%d SALA%d %s \n", k+1, 
   tavoli_possibili[k].codice_tavolo,tavoli_possibili[k].sala,tavoli_possibili[k].vicinanza);

  
   strcat(buftemp, buffer);
   
   }
   strcpy(buffer, buftemp);

    len=strlen(buffer)+1;
   lmsg= htons(len);
                        

  ret=send(i, (void*)&lmsg,sizeof(uint16_t),0);
 
 
   ret=send(i,(void*)buffer,len,0);
                        }
     if(strcmp(cmd, "book")==0){
   char prova[2];
   
   int prova2=0;
// Ricezione della dimensione del mesaggio 
ret = recv(i, (void*)&lmsg, sizeof(uint16_t), 0);
// Conversione in formato 'host'
len = ntohs(lmsg); 
// Ricezione del messaggio
ret = recv(i, (void*)prova, len, 0);

sscanf(prova, "%d",&prova2);

   fflush(stdout);


prova2--;
   
   fflush(stdout);
   // prenoto i tavoli settando i posti a 0
   book(libro_prenotazioni, tavoli_possibili,p, prova2, &codice);
  // salvo le prenotazioni in file prenotazioni.txt
   salva_prenotazioni(prenotaizioni,p,&count,&codice,code);
// salvo le modifiche del libro delle prenotazioni
   salva_libro(libro_prenotazioni);
   
  


   strcpy(buffer, "Prenotazione effettuata!");

   len=strlen(buffer);
  
   ret=send(i,(void*)buffer,len,0);
 
  ret=send(i, (void*)code,3,0);
  trova_socket('T',table_sd,numdevice,&t_count);
  fflush(stdout);
  //mando il codice al primo tavolo che trovo 
  send(table_sd[in_tavola],(void*)&code,strlen(code)+1,0);
  t[in_tavola].socket=table_sd[in_tavola];
  t[in_tavola].tavolo=codice;
  
  // incremento l'indice il prossimo codice sarà per il prossimo tavolo prenotato   
  in_tavola++;   
              fflush(stdout);


  


 
    
   if(ret<0){
    perror("Errore invio tavoli disponibili");
    exit(0);
   }
    }
       }
                        if(type=='K'){
                    

                ret=recv(i, (void*)&lmsgk, sizeof(uint8_t),0);
                ret=recv(i, (void*)cmd,lmsgk,0);
                 
       if(ret==0){

                        printf("CHIUSURA client rilevata! \n");
                        fflush(stdout);
                        close(i);
                        FD_CLR(i, &master);
                    } 
          else if(ret<0){
                        perror("Errore ricezione  msg \n");
                        close(i);
                          FD_CLR(i, &master);
                        
                    } 
              


             
                if(strcmp(cmd,"take")==0){
                  memset(bufferk,0,sizeof(bufferk));
                  int ii;
                  int numcomanda;
                  int jj;
                  int proviamoci=0;
                   char bufktemp[BUFFER_SIZE];
                  
                  
                  // guardo il vettore comande 
                  for(ii=0; ii<count_comande;ii++){
                    
                    // il primo con stato in attesa 
                    if(c[ii].status==0){
                      proviamoci++;
                      c[ii].status=1;
           
                      // inserisco numero della comanda
                     numcomanda=ii;
                
                      sprintf(bufktemp,"%d \n", numcomanda);
                      // lo inserisco nel buffer da mandare al kitchen 
                      uint8_t mando_tavolo;
                      mando_tavolo=c[ii].codice_tavolo;
                      send(i,(void*)&mando_tavolo,sizeof(uint8_t),0);
                      // trovo il tavolo il quale deve sapere della modifica dello stato della comanda 
                        t_sd=trova_socket_table(c[ii].codice_tavolo,t);
                  for(jj=0; jj<c[ii].n_ordini; jj++){
                   
                  sprintf(bufferk,"%s %d \n",c[ii].piatti_in_corso[jj].codice,c[ii].quanti[jj]);
                 
                  strcat(bufktemp, bufferk);
              
                  
                  
                  }
                
                
                  }
                  if(proviamoci==1)
                  break;
                   
                  }
                  strcpy(bufferk,bufktemp);
                 int preparazione= bufferk[0]-49;
                 uint8_t mando;
                 mando=preparazione;

        
                 lmsgk=strlen(bufferk);
                 ret=send(i,(void*)&lmsgk,sizeof(uint8_t),0);
                 ret=send (i, (void*)bufferk,lmsgk,0);
                 lmsgk=strlen(bufferk)+1;
                 
                  
  fflush(stdout);




 stato=c[numcomanda].status;

// mando al tavolo la comanda che è in preparazione 
  ret=send(t_sd,(void*)&lmsgk,sizeof(uint8_t),0);
  ret=send(t_sd,(void*)bufferk,lmsgk,0);

    ret=send(t_sd,(void*)&stato,sizeof(uint8_t),0);




                }
                if(strcmp(cmd,"ready")==0){
                  uint8_t ricevo;
                  int in_servizio;
                  int tav;
                // ricevo la comanda pronta
                  ret=recv(i,(void*)&lmsgk,sizeof(uint8_t),0);
                
                  ret=recv(i,(void*)bufferk,lmsgk,0);
                  ret=recv(i,(void*)&ricevo,sizeof(uint8_t),0);
                  printf("%s",bufferk);
                
                  tav=ricevo;
                  in_servizio=bufferk[0]-48;
                 
                  

                
                  c[in_servizio].status=2;
        
                t_sd=trova_socket_table(tav,t);
                    stato=2;
              


  ricevo=in_servizio;
  // rimando direttamente al tavolo corrispondente la comanda in servizio
  ret=send(t_sd,(void*)&lmsgk,sizeof(uint8_t),0);
  ret=send(t_sd,(void*)bufferk,lmsgk,0);
// invio anche lo stato perchè il tavolo non sa se gli arriva una comanda in preparazione o pronta 
    ret=send(t_sd,(void*)&stato,sizeof(uint8_t),0);
  


                
             



  }
            
        }
    
        }
             
    
          
}
}
}
printf("CHIUDO IL LISTENER");
fflush(stdout);
close(listener);

}