#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define MAX_PRENOTAZIONI 100 
#define DATA_DIM 10
#define BUFFER_SIZE 1024
#define DIM_VICINANZA 30
#define GIORNIPRENOTABILI 7
#define ORARIMAX 6
#define TAVOLIMAX 30
// strutture annidte  per gestire le prenotazioni 
// abbiamo un libro prenotazioni che è un array di giorni prenotabili 
// ogni giorno prenotabile ha degli orari disponibili
// ogni orario disponibile ha una serie di tavoli 
struct tm*timeinfo;
time_t rawtime;
typedef struct {
    int ndisponibili; // posti disponibili 2/4/6 
    int codice_tavolo; // numero del tavolo
    int sala; // 1 2 3 
    char vicinanza[DIM_VICINANZA]; // informazioni sul tavolo

} tavolo ;
typedef struct {
    int ora;
    tavolo tavoli_disponibili[TAVOLIMAX];


} orari_disponibili;
typedef struct {
    char giorno[DATA_DIM];
orari_disponibili giornoX[ORARIMAX];

} giorno_feriale;
typedef struct{
  char cognome[30];
 
int  numero;

  char data[10];
 int ora;
 char codiceprenotazione[3];
 char timestamp[9];


} prenotazione;

// inizializziamo il libro delle prenotazioni
// usiamo la settimana corrente al 14/02/2023 al 20/02/2023
// gli orari disponibili sono pranzo: 12 13 14 e cena: 19 20 21 
// il codice è commentato perchè il server legge direttamente il file libro_prenotazione.txt per iniziallizzare le strutture dati
/*
void inilibro(giorno_feriale*l){
    int i,j,k;
    for(i=0; i<GIORNIPRENOTABILI; i++){
    
    char g=i+52;
    l[i].giorno[0]='1';
        l[i].giorno[1]=g;
        if(g==58){
            l[i].giorno[0]='2';
            l[i].giorno[1]='0';
        }
            if(g==59){
            l[i].giorno[0]='2';
            l[i].giorno[1]='1';
            }
        
        strcat(l[i].giorno, "-02-23");
     
        for(j=0; j<ORARIMAX; j++){
            if(j<3)
            l[i].giornoX[j].ora=12+j;
            else 
            l[i].giornoX[j].ora=19+(j-3);
           for( k=0; k<TAVOLIMAX;k++){
            l[i].giornoX[j].tavoli_disponibili[k].codice_tavolo=k;
            if(k<10)
            l[i].giornoX[j].tavoli_disponibili[k].sala=1;
            if(k>=10 && k<20)
            l[i].giornoX[j].tavoli_disponibili[k].sala=2;
             if(k>=20)
             l[i].giornoX[j].tavoli_disponibili[k].sala=3;
             if(k<6 || (k>=10 && k<16) || (k>=20 && k<26))
              l[i].giornoX[j].tavoli_disponibili[k].ndisponibili=2;
              if(k>=6 && k<8 || (k>=16 && k<18) || (k>=26 && k<28))
              l[i].giornoX[j].tavoli_disponibili[k].ndisponibili=4;
              if(k>=8 && k<10 || (k>=18 && k<20) || (k>=28))
              l[i].giornoX[j].tavoli_disponibili[k].ndisponibili=6;
              if(k%2==0)
              strcpy(l[i].giornoX[j].tavoli_disponibili[k].vicinanza,"CAMINO");
              if(k%3==0)
              strcpy(l[i].giornoX[j].tavoli_disponibili[k].vicinanza,"TAVOLO_INTERNO");
              if(k%5==0)
              strcpy(l[i].giornoX[j].tavoli_disponibili[k].vicinanza,"FINESTRA");
              if(k==1 || k==7)
              strcpy(l[i].giornoX[j].tavoli_disponibili[k].vicinanza,"INGRESSO");
              if(k==11 || k== 13 || k== 17 || k== 19)
              strcpy(l[i].giornoX[j].tavoli_disponibili[k].vicinanza,"BAGNI");
              if(k==23 || k==29)
              strcpy(l[i].giornoX[j].tavoli_disponibili[k].vicinanza,"CUCINA");
              






           }
        }
 
     
  
    } 
  
   

} */
// scrive nel file libro_prenotazioni.txt l'array giorni_disponibili nel formato:
/*
GIORNO 
ORA 
Codice tavolo, posti, sala, vicinanza 
*/
void salva_libro(giorno_feriale*l){
    int i,j,k;
    FILE* fptr;
    fptr=fopen("libro_prenotazioni.txt","w");
    if(fptr==NULL){
        printf("Error!");
        exit(1);
    }
    for(i=0; i<GIORNIPRENOTABILI; i++){
        fprintf(fptr,"%s \n", l[i].giorno);
    
        for(j=0; j<ORARIMAX; j++){
            fprintf(fptr,"%d \n", l[i].giornoX[j].ora);
         for( k=0; k<TAVOLIMAX;k++){
            fprintf(fptr,"%d %d %d %s \n",l[i].giornoX[j].tavoli_disponibili[k].codice_tavolo
            , l[i].giornoX[j].tavoli_disponibili[k].ndisponibili,l[i].giornoX[j].tavoli_disponibili[k].sala,
            l[i].giornoX[j].tavoli_disponibili[k].vicinanza);
         }

        }
        
    
}
fclose(fptr);
}
// salva in un array e in un file le prenotazioni comfermate 
// nel formato cognome, posti codiceprenotazione, data e ora 
 void salva_prenotazioni(prenotazione*p1,prenotazione p, int*count, int* codice, char*code){
   
    char cod[1];
    char timer[9];
    time(&rawtime);
    timeinfo=localtime(&rawtime);
    sprintf(timer," %d %d %d %d %d %d",(timeinfo->tm_year)-100,(timeinfo->tm_mon)+1,timeinfo->tm_mday,
    timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    
   strcpy(p1[*count].timestamp,timer);
    cod[0]=p.cognome[0];
    cod[1]='\0';
    char codvalue[3];
 
    sprintf(codvalue,"%d", *codice);
      
    strcat(cod,codvalue);
    strcpy(p1[*count].codiceprenotazione,cod);

   strcpy(p1[*count].cognome,p.cognome);
    strcpy(p1[*count].data,p.data);
    p1[*count].ora=p.ora;
    p1[*count].numero=p.numero;

    strcpy(p1[*count].codiceprenotazione,cod);
   strcpy(code,cod);
    
    
    FILE*fptr;
    fptr=fopen("prenotaizioni.txt","w");
    if(fptr == NULL)
   {
      printf("Error!");   
      exit(1);             
   }
   int i;
   (*count)++;
   for(i=0; i<(*count); i++){

    fprintf(fptr,"%s %d %s  %s %d %s \n",p1[i].cognome, p1[i].numero,p1[i].codiceprenotazione,
    p1[i].data, p1[i].ora, p1[i].timestamp);

   }






   }
   // legge il file libro_prenotazione.txt per sovrascrivere l'array l
void leggi_libro(giorno_feriale*l){
    FILE*fptr;
    if((fptr = fopen("libro_prenotazioni.txt","r")) == NULL){
       printf("Error! opening file libro \n");

       // Program exits if the file pointer returns NULL.
       exit(1);
   }
   int i,j,k;
   
    for(i=0; i<GIORNIPRENOTABILI; i++){
      fscanf(fptr,"%s",l[i].giorno);
    
        for(j=0; j<ORARIMAX; j++){
       fscanf(fptr, "%d",&l[i].giornoX[j].ora );
         for( k=0; k<TAVOLIMAX;k++){
            fscanf(fptr,"prenotazioni: \n");
   fscanf(fptr,"%d %d %d %s", &l[i].giornoX[j].tavoli_disponibili[k].codice_tavolo,
     &l[i].giornoX[j].tavoli_disponibili[k].ndisponibili,  &l[i].giornoX[j].tavoli_disponibili[k].sala,
      l[i].giornoX[j].tavoli_disponibili[k].vicinanza);
         }
        }
    }







} 
// inserisce in tavoli possibili i tavoli disponibili alla richiesta della prenotazione rispetto al libro_prenotazioni
void prenota(giorno_feriale*l,tavolo*tavoli_possibili,prenotazione p,int* dim){

    

    int i,j,k;
    int q=0;
//svuoto tavoli possibili 

    for(i=0; i<GIORNIPRENOTABILI; i++){
        if(strcmp(l[i].giorno, p.data)==0){
        for(j=0; j<ORARIMAX; j++){
            if(l[i].giornoX[j].ora==p.ora){
                for(k=0; k<TAVOLIMAX; k++){
                    if(l[i].giornoX[j].tavoli_disponibili[k].ndisponibili==p.numero){
                    tavoli_possibili[q].ndisponibili=l[i].giornoX[j].tavoli_disponibili[k].ndisponibili;
                    
                    
                    tavoli_possibili[q].codice_tavolo=l[i].giornoX[j].tavoli_disponibili[k].codice_tavolo;

                    tavoli_possibili[q].sala=l[i].giornoX[j].tavoli_disponibili[k].sala;
                    strcpy(tavoli_possibili[q].vicinanza,l[i].giornoX[j].tavoli_disponibili[k].vicinanza );
                   
                       
                         q++;
                    }



                }
            }
        }

        }
    }
    *dim=q;
   

} 
// modifico a 0 i posti del tavolo scelto da choice e in codice mi salvo il numero del tavolo 
// il codice poi della table sarà la prima lettera del cognome della prenotazione susseguita dal numero del tavolo
void book(giorno_feriale* l, tavolo* tavoli_possibili,prenotazione p,int choice, int*codice){
    int i,j,k;
  for(i=0; i<GIORNIPRENOTABILI; i++){
     if(strcmp(l[i].giorno, p.data)==0){
        for(j=0; j<ORARIMAX; j++){
            if(l[i].giornoX[j].ora==p.ora){
                for(k=0; k<TAVOLIMAX; k++){
// se il tavolo è stato richiesto ed è disponibile viene occupato 
  if(tavoli_possibili[choice].codice_tavolo==l[i].giornoX[j].tavoli_disponibili[k].codice_tavolo){

    *codice=l[i].giornoX[j].tavoli_disponibili[k].codice_tavolo;
l[i].giornoX[j].tavoli_disponibili[k].ndisponibili=0;

  }
   }
            }
        }
     }
  }
}

