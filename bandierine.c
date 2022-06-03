/* CONSEGNA *

Il sistema modella un turno del gioco delle bandierine.
Ci sono due giocatori che al "via!" devono tentare di prendere la bandierina tenuta dal giudice.
Il primo che riesce a prendere la bandierina deve ritornare alla base, mentre l'altro deve cercare di raggiungerlo.
Il sistema è modellato da tre thread: uno per il giudice, ed uno per ogni giocatore.
*/



/* PREMESSA *

Il programma consegnato su carta, se corretti gli errori di trascrizione (1) e (2) sotto riportati,
dovrebbe funzionare correttamente (eseguendolo si sono verificati tutti i casi).

L'unico problema logico del programma consegnato su carta (e per la versione "corr_v1"), è che gli identificativi dei giocatori potevano essere 
solo numeri maggiori di 0, allora per risolvere questa limitazione ho modificato le variabili nella struttura dati 
(e di conseguenza modificato leggermente i controlli che le utilizzavano).

P.S.: tutta la logica dei semafori è rimasta invariata.

In ogni caso è tutto indicato di seguito
*/



/* CORREZIONI *
--> Errori
(1) Riga 202: Mancava la "sem_post(&b->svia)" per liberare il secondo giocatore nella funzione "via"        (Errore di distrazione durante la trascrizione dalla brutta alla bella)

(2) Riga 222: Avevo ri-dichiarato la variabile "res" scrivendo "int" davanti nella funzione "ti_ho_preso"   (Errore di distrazione)

--> Miglioramenti
(3) Rimosse le variabili ""booleane"" singole per i giocatori (nb1 e nb2) e introdotto un singolo contatore (ng) per determinare il numero di giocatori pronti

(4) Aggiunte variabili ""booleane"" (int 0 o 1) per determinare se:
    la bandiera è stata presa ("bandiera_presa"), qualcuno è alla base ("qualcuno_alla_base"), qualcuno ha preso l'altro ("qualcuno_ha_preso")

(5) Per determinare chi vince rimosse le variabili "presa", "preso_da", "alla_base" e sostituite con un unica variabile "vincitore"
*/



/* AGGIUNTE PER L'EFFETTIVO FUNZIONAMENTO *

- Il main
- Funzione per una pausa randomica "pausa_random"
- Funzione per una pausa determinata in ms "pausa_ms"
- Modificati i "printf" per rendere più leggibile e human-friendly l'output
*/



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

struct bandierine_t {
    sem_t mutex;            // Semaforo per il mutex
    sem_t sg1;              // Semaforo per il giocatore 1
    sem_t sg2;              // Semaforo per il giocatore 2
    sem_t svia;             // Semaforo per il via
    sem_t sstop;            // Semaforo per lo stop
    
    int bandiera_presa;     // Variabile per capire SE qualcuno ha preso la bandiera
    int qualcuno_alla_base; // Variabile per capire SE qualcuno è alla base
    int qualcuno_ha_preso;  // Variabile per capire SE qualcuno ha preso l'altro

    int vincitore;          // Variabile dinamica che tiene traccia di chi sta vincendo in un determinato momento                  

    int ng;                 // Variabile per determinare numero di giocatori pronti

} bandierine;

void init_bandierine(struct bandierine_t *b);
void via (struct bandierine_t *b);
void attendi_il_via (struct bandierine_t *b, int n);
int bandierina_presa (struct bandierine_t *b, int n);
int sono_salvo (struct bandierine_t *b, int n);
int ti_ho_preso (struct bandierine_t *b, int n);
int risultato_gioco (struct bandierine_t *b);
void attendi_giocatori (struct bandierine_t *b);
void pausa_random(int max_ms);
void pausa_ms(int ms);

void *giocatore(void *arg){
    int numerogiocatore = (long)arg;                // (int) dava un warning durante la compilazione
    pausa_random(500);                              // Metto una pausa qua per fare in modo che il giocatore possa arrivare dopo il giudice
    
    attendi_il_via(&bandierine, numerogiocatore);
    printf("Giocatore %d: Partito!\n", numerogiocatore);

    pausa_random(1000);   // <corri e tenta di prendere la bandierina> (attendo un tempo casuale)

    if (bandierina_presa(&bandierine, numerogiocatore)){
        printf("\nGiocatore %d: Bandierina presa!\n", numerogiocatore);

        pausa_random(1000); // <corri alla base> (attendo un tempo casuale)
        
        if (sono_salvo(&bandierine, numerogiocatore)) printf("Giocatore %d: Salvo!\n\n", numerogiocatore);
    
    } else {
        
        pausa_random(1000); // <cerca di acchiappare l'altro giocatore> (attendo un tempo casuale)
        
        if (ti_ho_preso(&bandierine, numerogiocatore)) printf("Giocatore %d: Ti ho preso!\n\n", numerogiocatore);
    }
    return 0;
}

void *giudice(void *arg){
    pausa_random(500);                      // Metto una pausa qua per fare in modo che possa capitare che il giudice arrivi dopo i giocatori
    printf("Giudice: Sono arrivato!\n");
    
    attendi_giocatori(&bandierine);         // Aspetto che ci siano entrambi i giocatori
    via(&bandierine);                       // <pronti, attenti, ...>
    
    pausa_ms(300);                          // Attendo un attimo prima di comunicare il risultato
    printf("Giudice: Il vincitore é il Giocatore %d!\n", risultato_gioco(&bandierine));
    return 0;
}


int main(int argc, char const *argv[]) {
    pthread_attr_t a;
    pthread_t p;

    /* inizializzo il gioco */
    init_bandierine(&bandierine);

    /* inizializzo il random con un seed temporale */
    srand(time(NULL));

    pthread_attr_init(&a);

    /* evito di fare le join */
    pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);

    pthread_create(&p, &a, giocatore, (void *)-3);   // Creo il giocatore 1 con ID 0
    pthread_create(&p, &a, giocatore, (void *)14);   // Creo il giocatore 2 con ID 1
    pthread_create(&p, &a, giudice, NULL);          // Creo il giudice

    pthread_attr_destroy(&a);

    sleep(5);

    return 0;
}


void init_bandierine (struct bandierine_t *b){
    sem_init(&b->mutex, 0, 1);                                              // Inizializzo il semaforo mutex a 1 (verde)
    sem_init(&b->sg1,   0, 0);                                              // Inizializzo il semaforo del giocatore 1 a 0 (rosso)
    sem_init(&b->sg2,   0, 0);                                              // Inizializzo il semaforo del giocatore 2 a 0 (rosso)
    sem_init(&b->svia,  0, 0);                                              // Inizializzo il semaforo del via a 0 (rosso)
    sem_init(&b->sstop, 0, 0);                                              // Inizializzo il semaforo dello stop a 0 (rosso)

    b->ng = 0;                                                              // Inizializzo la variabile per contare il numero di giocatori pronti
    b->bandiera_presa = b->qualcuno_alla_base = b->qualcuno_ha_preso = 0;   // Inizializzo le variabili per determinare SE qualcuno ha fatto una delle azioni a 0
    b->vincitore = 0;                                                       // Inizializzo la variabile del vincitore a 0 (a caso)
}

void attendi_il_via (struct bandierine_t *b, int n){
    sem_wait(&b->mutex);        // Prendo il semaforo mutex per evitare che qualcuno possa scrivere (o leggere) contemporaneamente e creare inconsistenze nella struttura dati
    if (b->ng == 0){            // Se non c'è ancora nessun giocatore
        b->ng++;                // Allora lo faccio io (giocatore 1)
        sem_post(&b->sg1);      // Sblocco il giudice dalla mia attesa

    } else if (b->ng == 1){     // Se invece non c'è ancora il secondo giocatore
        b->ng++;                // Allora lo faccio io (giocatore 2)
        sem_post(&b->sg2);      // Sblocco il giudice dalla mia attesa
    }
    sem_post(&b->mutex);        // Rilascio il semaforo mutex
    sem_wait(&b->svia);         // Rimango in attesa del via da parte del giudice
}

void attendi_giocatori (struct bandierine_t *b){
    printf("Giudice: In attesa dei giocatori...\n");
    
    sem_wait(&b->mutex);
    if (b->ng == 2){                        // Se sono arrivati entrambi i giocatori
        sem_post(&b->sg1);                  // Libero il giocatore 1 
        sem_post(&b->sg2);                  // e il giocatore 2
    }
    sem_post(&b->mutex);
    
    sem_wait(&b->sg1);                      // Se non era arrivato il giocatore 1 lo attendo qui
    printf("Giudice: Primo Giocatore arrivato\n");
    
    sem_wait(&b->sg2);                      // Se non era arrivato il giocatore 2 lo attendo qui
    printf("Giudice: Secondo Giocatore arrivato\n");
}

void via (struct bandierine_t *b){
    printf("Giudice: Via!\n\n");
    pausa_ms(10);
    
    sem_post(&b->svia);     // Libera un giocatore
    sem_post(&b->svia);     // Libera l'altro giocatore
    sem_wait(&b->sstop);    // Giudice si mette in attesa per la fine del gioco
}

int bandierina_presa (struct bandierine_t *b, int n){
    sem_wait(&b->mutex);
    int res = 0;
    if (b->bandiera_presa == 0){    // Se la bandiera non è ancora stata presa da nessuno (Modificato questo controllo)
        b->bandiera_presa = 1;      // Flaggo la bandiera come presa
        b->vincitore = n;           // La prendo io (mettendo il mio identificativo)
        res = 1;                    // Poi ritorno "True"
    }
    sem_post(&b->mutex);
    return res;
}

int ti_ho_preso (struct bandierine_t *b, int n){
    sem_wait(&b->mutex);
    int res = 0;
    if ((b->qualcuno_alla_base == 0) && (b->qualcuno_ha_preso == 0)){   // Se nessuno è alla base e nessuno è stato ancora preso (Modificato questo controllo)
        res = 1;                                                        // Poi ritorno "True" (--> Qua avevo messo un "int" di troppo)        
        b->qualcuno_ha_preso = 1;                                       // Flaggo che qualcuno è stato preso
        b->vincitore = n;                                               // Acchiappo io
        sem_post(&b->sstop);                                            // Fine del gioco, sblocco il giudice
    }
    sem_post(&b->mutex);
    return res;
}

int sono_salvo (struct bandierine_t *b, int n){
    sem_wait(&b->mutex);
    int res = 0;
    if ((b->qualcuno_alla_base == 0) && (b->qualcuno_ha_preso == 0)){   // Se non c'è nessuno alla base e nessuno è stato preso (Modificato questo controllo)
        b->qualcuno_alla_base = 1;                                      // Flaggo che qualcuno è arrivato alla base
        b->vincitore = n;                                               // Sono arrivato alla base
        res = 1;                                                        // Poi ritorno "True"  
        sem_post(&b->sstop);                                            // Fine del gioco, sblocco il giudice
    }
    sem_post(&b->mutex);
    return res;
}

int risultato_gioco (struct bandierine_t *b){
    sem_wait(&b->mutex);
    int res = b->vincitore;
    sem_post(&b->mutex);
    return res;
}

/* Utils */
void pausa_random(int max_ms) {
  struct timespec t;
  t.tv_sec = 0;
  t.tv_nsec = (rand() % max_ms + 1) * 1000000; // da 1 a 10 (moltiplicato per 1.000.000 in modo da ottenere nanosecondi. ex: 1000000ns = 1ms) ==> 1-10ms
  //printf("%ldns", t.tv_nsec);
  nanosleep(&t,NULL);
}
void pausa_ms(int ms){
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec = ms*1000000; //ms -> ns
    nanosleep(&t,NULL);
}