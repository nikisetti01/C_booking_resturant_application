#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "prenotazioni.c"
#include "connessioni.c"

void carica_device(device*d, int numdevice);
void trova_socket(char types, int*type_sd, int numdevice,int* count);
int  leggi_tipo(int newsocket, char* newtype, int numdevice);

void inilibro(giorno_feriale*l);
void salva_libro(giorno_feriale*l);
void salva_prenotazioni(prenotazione*p1,prenotazione p, int*count, int* codice, char*code);
void leggi_libro(giorno_feriale*l);
void prenota(giorno_feriale*l,tavolo*tavoli_possibili,prenotazione p,int* dim);
void book(giorno_feriale* l, tavolo* tavoli_possibili,prenotazione p,int choice, int*codice);

