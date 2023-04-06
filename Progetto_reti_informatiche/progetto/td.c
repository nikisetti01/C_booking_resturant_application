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
#define DIM_CODE 4
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
} comanda;
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
// funzione che trova per ogni piatto il suo prezzo
void trova_costo(piatto *p,piatto*m){
  int i;
  for(i=0; i<NUM_PIATTI; i++){
    
    if(strcmp(m[i].codice,p->codice)==0){
    p->prezzo=m[i].prezzo;
   
    }
  }
  

}
// inizzializzo il menu in modo permanente 
void inimenu(piatto*m){
 



   strcpy(m[0].codice, "A1");
 
    strcpy(m[0].nome,"Fantasia_di_mare");
     m[0].prezzo=10;
   
    strcpy(m[1].nome,"Insalata_di_Mare");
  m[1].prezzo=8;
     strcpy(m[1].codice, "A2");
 
    strcpy(m[2].nome, "Spaghetto_allo_scoglio");
    m[2].prezzo=11;
     strcpy(m[2].codice, "P1");
    
    strcpy(m[3].nome, "Linguine_all_astice");
    m[3].prezzo=18;
     strcpy(m[3].codice, "P2");
  
    strcpy(m[4].nome, "Frittura_di_mare");
    m[4].prezzo=15;
     strcpy(m[4].codice, "S1");
    
    strcpy(m[5].nome, "Orata_al_forno");
    m[5].prezzo=11;
     strcpy(m[5].codice, "S2");
    
    strcpy(m[6].nome, "Mattonella_ai_pinoli");
    m[6].prezzo=5;
     strcpy(m[6].codice, "D1");


    strcpy(m[7].nome, "Panna_cotta");
    m[7].prezzo=5;
     strcpy(m[7].codice, "D2");
  

   

  
  

  
  
 

}
void stampa_menu(piatto*m){

  int i;
  for(i=0; i<NUM_PIATTI;i++)
  printf("%s - %s %d \n",m[i].codice,m[i].nome,m[i].prezzo);

} 
int main(int argc, char* argv[]){

int conto_stato=0;
    char code[2];
    char type='T';
  char control[2];
    int count_comande=0;
  piatto menu[NUM_PIATTI];
  inimenu(menu);
uint8_t lmsg;

  int conto=0;

  comanda c[MAX_COMANDA];

 char bufferk[BUFFER_SIZE];
      int i,ret, sd,fdmax;
      fd_set read_fds;
fd_set master;
      char cmd[10];
        char buffer[BUFFER_SIZE];
         struct sockaddr_in srv_addr;
         int num_tavolo;
           sd=socket(AF_INET,SOCK_STREAM,0);
    memset(&srv_addr, 0, sizeof(srv_addr));
      srv_addr.sin_family = AF_INET;
srv_addr.sin_port = htons(4242/*atoi(argv[1])*/);
inet_pton(AF_INET, "127.0.0.1", &srv_addr.sin_addr);
  ret=connect(sd, (struct sockaddr*)&srv_addr, sizeof(srv_addr));
  char buftype[2];
  buftype[0]=type;
  // mando il tipo al server 
  ret=send(sd, (void*)buftype,2,0);
  if(ret<0){
    perror("Problemi di connessione");
    exit(0);
  }

  // inizializzo i fd dentro master;
FD_SET(sd,&master);
    FD_SET(0,&master);
    fdmax=sd;
    


printf("aspetto il codice \n");
       ret=recv(sd, (void*)code,DIM_CODE,0);
       char t_temp[3];
       strcpy(t_temp,code);
       t_temp[0]='T';
       // dico il numero del tavolo e richiedo il codice 
    printf("%s inserire coidce \n", t_temp);
    scanf("%s", control);
    if(strcmp(control,code)==0){
        printf("***************************** BENVENUTO ***************************** \n");
            printf("digita un comando \n");
            printf("1) help --> mostra i dettagli dei comandi \n");
            printf("2) menu --> mostra il menu dei piatti \n");
            printf("3) comanda --> invia una comanda \n");
            printf("4) conto --> chiede il conto \n");
    }

    


     

          while(1){
                
                
  read_fds=master;
        ret=select(fdmax+1,&read_fds,NULL,NULL,NULL);
  
   if(ret<0){
            perror("ERRORE SELECT DEL KITCHEN");
            exit(1);
        }
         for(i=0; i<=fdmax; i++){
            if(FD_ISSET(i,&read_fds)){
                if(i==0){
                       if(strcmp(control,code)==0){
          
            
                   scanf("%s", cmd);
     lmsg=strlen(cmd)+1;
// la mando al server
  ret=send(sd, (void*)&lmsg,sizeof(uint8_t),0);
   ret=send(sd,(void*)cmd,lmsg,0); 
    if(strcmp(cmd,"comanda")==0){
    

      
//prendo la comanda 
memset(buffer,0,BUFFER_SIZE);
 fgets(buffer,BUFFER_SIZE,stdin);

    lmsg=strlen(buffer)+1;
  
  
// la mando al server


  ret=send(sd, (void*)&lmsg,sizeof(uint8_t),0);
   ret=send(sd,(void*)buffer,lmsg,0);
   // parsing delle comande 
      int counter=0;
                int cursor=1;
               
                while(buffer[counter]!='\0'){
                  c[count_comande].piatti_in_corso[counter].codice[0]=buffer[cursor];
                  cursor++;
                          
                  c[count_comande].piatti_in_corso[counter].codice[1]=buffer[cursor];
                  cursor+=2;
                  c[count_comande].quanti[counter]=buffer[cursor]-48;
                  counter++;
                  cursor+=2;
                }
                 
                c[count_comande].n_ordini=counter/5;
             
                int l;
                
              
            
                for(l=0; l<c[count_comande].n_ordini;l++){
                // per ogni piatto selezionato associo il relativo prezzo
                trova_costo(&c[count_comande].piatti_in_corso[l],menu);
             
                } 
              count_comande++;
         
   char buffert[BUFFER_SIZE]; 
   ret=recv(sd,(void*)buffert,DIM_COMF_COMANDA,0);
  buffert[DIM_COMF_COMANDA-1]='\0';
   printf(" %s \n", buffert);
 // ricevo il buffer con le comande in stato di attesa 

   ret=recv(sd,(void*)&lmsg,sizeof(uint8_t),0);
  ret=recv(sd,(void*)buffert,lmsg,0);
  printf("c%s", buffert);

      
    }
    if(strcmp(cmd,"help")==0){
      printf("menu \n");
      printf("comanda {<piatto_1-quantità1>...piatto_n-quantità_n>} \n");
      printf("conto\n");

    }
      if(strcmp(cmd,"menu")==0){
     stampa_menu(menu);

    }
    if(strcmp(cmd,"conto")==0){
      int conto_possibile=0;
      int real_price;
      conto=0;
      int l,k;
      for(l=0; l<count_comande; l++){
        // se tutte le comande ricevute le ho ricevute 2 volte (preparazione e servizio) allora faccio il conto
        if(conto_stato !=(count_comande*2))
          conto_possibile=1;
        for(k=0; k<c[l].n_ordini; k++){
          if(conto_possibile==0){
            // prezzo per quantità
           real_price=(c[l].quanti[k])*(c[l].piatti_in_corso[k].prezzo);
          printf(" %s  %d %d \n",c[l].piatti_in_corso[k].codice,c[l].quanti[k], real_price);
          conto+=real_price;
          }
        }
        

        
      }
      if(conto_possibile==0){
        // mostro il conto e spengo il device 
      printf(" Totale:%d \n", conto);
       close(sd);
         FD_CLR(sd, &master);
         FD_CLR(i,&master);
         exit(0);
      }
      else 
      printf("Impossibile effettuare il conto \n");

     

    }


  }
                }else 
  { 
    memset(bufferk,0,sizeof(bufferk));
     uint8_t stato;
  int k_index;
    //gestisco la ricezione delle comande 
memset(bufferk,0,sizeof(bufferk));
// ricevo nel buffer tutta la comanda 
  ret=recv(sd,(void*)&lmsg,sizeof(uint8_t),0);
  ret=recv(sd,(void*)bufferk,lmsg,0);
     if(strcmp(bufferk,"stop")==0){
      printf("Chiusura di tutti i device \n");
      close(sd);
         FD_CLR(sd, &master);
         FD_CLR(i,&master);
         exit(0);
    }
    // ricevo lo stato per capire se la comanda ricevuta è in preparazione o in servizio
  ret=recv(sd,(void*)&stato,sizeof(uint8_t),0);
  k_index=bufferk[0]-48;
bufferk[0]=' ';
   fflush(stdout);


              
               printf("comanda %d: ", k_index+1);
              converti_status(stato);
              printf("%s",bufferk);
              conto_stato++;
             
               




  
  }


  
}
         }
                 }
                 return 0;
          }

