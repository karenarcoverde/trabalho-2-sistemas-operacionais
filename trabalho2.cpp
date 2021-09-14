//gcc -Wall -pthread 

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <list>

#define CLIENT_NUMBER 10

#define SIZE_FILA 5
using namespace std;



int finalizou = 0;
int num_chairs = 10;



typedef struct {
   pthread_mutex_t mutex;
   pthread_cond_t  cond;
} cliente_struct;



pthread_mutex_t clientArray_mutex = PTHREAD_MUTEX_INITIALIZER;
cliente_struct clientes[CLIENT_NUMBER];
int clientePos=0;

pthread_mutex_t cadeiras_mutex = PTHREAD_MUTEX_INITIALIZER;
list<int> cadeiras;


pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t client_ = PTHREAD_COND_INITIALIZER;


//pthread_mutex_t barber_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t barber_ = PTHREAD_COND_INITIALIZER;



//for exit and join
pthread_mutex_t finishMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t finishCond = PTHREAD_COND_INITIALIZER;



void thr_join_all(unsigned size) {
    pthread_mutex_lock(&finishMutex);
    while (finalizou != size)
        pthread_cond_wait(&finishCond, &finishMutex);
    pthread_mutex_unlock(&finishMutex);
    printf("Todas as threads terminaram\n");
}


void *barber( void *) {
  
   cliente_struct *currentCliente;
   while(1) {
      
      /* wait for a client to become available (sem_client) */
	  pthread_cond_wait(&client_,&client_mutex);
      /* wait for mutex to access chair count (chair_mutex) */
      currentCliente = &clientes[cadeiras.front()];
      printf("Barber: atendendo cliente na posicao %i\n",cadeiras.front());
	  
      sleep(1);
      /* signal to client that barber is ready to cut their hair (sem_barber) */
	  pthread_cond_signal(&currentCliente->cond);
      /* give up lock on chair count */

      
      
      pthread_mutex_lock(&cadeiras_mutex);
      printf("Barber: cabelo cortado \n");
      /* increment number of chairs available */
	   cadeiras.pop_front();
	   pthread_mutex_unlock(&cadeiras_mutex);

      /* cut hair for worktime seconds (really just call sleep()) */
     
	
    } 
}

void *client(void *) {

   int posCopy;
   pthread_mutex_lock(&clientArray_mutex);
   posCopy = clientePos;
   clientes[posCopy].mutex=PTHREAD_MUTEX_INITIALIZER;
   clientes[posCopy].cond=PTHREAD_COND_INITIALIZER;
   clientePos++;
   pthread_mutex_unlock(&clientArray_mutex);

   while(1) {
      /* wait for mutex to access chair count (chair_mutex) */
	  pthread_mutex_lock(&cadeiras_mutex);
      /* if there are no chairs */
	  if(cadeiras.size() >= SIZE_FILA){
           /* free mutex lock on chair count */
		   printf("Client: Thread  %lu(%i) leaving with no haircut\n", pthread_self(),posCopy);
		   pthread_mutex_unlock(&cadeiras_mutex);
	  }
      /* else if there are chairs */
	  else{
           /* decrement number of chairs available */
         cadeiras.push_back(posCopy);

		   printf("Client: Thread  %lu(%i) Sitting to wait. Number of chairs left = %d\n",pthread_self(),posCopy,cadeiras.size());
         pthread_mutex_unlock(&cadeiras_mutex);

           /* signal that a customer is ready (sem_client) */
		   pthread_cond_signal(&client_);
           /* free mutex lock on chair count */
		   
           /* wait for barber (sem_barber) */
		   pthread_cond_wait(&clientes[posCopy].cond,&clientes[posCopy].mutex);
           /* get haircut */
		   printf("Client: Thread  %lu(%i) getting a haircut\n",pthread_self(),posCopy);
	  }
     sleep(1);
     printf("Client: Thread %lu(%i) waiting seconds before attempting next haircut\n",pthread_self(),posCopy);
   }


     return NULL;
}

int main() {

   pthread_t threadsall [CLIENT_NUMBER+1];

   pthread_create(&threadsall[0], NULL, barber, NULL);
   //printf("Creating barber thread with id %lu\n",threadsall[0]);

   /* 4. Create client threads.  */
   for (int i = 1; i < CLIENT_NUMBER+1; i++){
	   pthread_create(&threadsall[i], NULL, client, NULL);
	   //printf("Creating client thread with id %lu\n",threadsall[i]);
   }

   thr_join_all (CLIENT_NUMBER);
   return 0;
}


