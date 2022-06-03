#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

struct morra_cinese_t {
    sem_t mutex;            // Semaforo per il mutex
    sem_t sg1;              // Semaforo per il giocatore 1
    sem_t sg2;              // Semaforo per il giocatore 2
    sem_t svia;             // Semaforo per il via
    sem_t sstop;            // Semaforo per lo stop
    
    //int bandiera_presa;     // Variabile per capire SE qualcuno ha preso la bandiera
    //int qualcuno_alla_base; // Variabile per capire SE qualcuno è alla base
    //int qualcuno_ha_preso;  // Variabile per capire SE qualcuno ha preso l'altro

    int round;              // Numero del round corrente

    int vincitore;          // Variabile dinamica che tiene traccia di chi sta vincendo in un determinato momento                  

    int ng;                 // Variabile per determinare numero di giocatori pronti

} bandierine;

void init_morra_cinese(struct morra_cinese_t *m);

int main(int argc, char const *argv[]) {

    return 0;
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