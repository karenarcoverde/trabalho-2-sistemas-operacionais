//gcc -Wall -pthread 
#define MAX 20
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int finalizou = 0;
int num_chairs = 10;

pthread_mutex_t chairs_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t client_ = PTHREAD_COND_INITIALIZER;
pthread_cond_t barber_ = PTHREAD_COND_INITIALIZER;

//for exit and join
pthread_mutex_t finishMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t finishCond = PTHREAD_COND_INITIALIZER;

//for leave , but never used
void thr_exit() {
     pthread_mutex_lock(&finishMutex);
     finalizou++;
     pthread_cond_signal(&finishCond);
     pthread_mutex_unlock(&finishMutex);
}

void thr_join_all(unsigned size) {
    pthread_mutex_lock(&finishMutex);
    while (finalizou != size)
        pthread_cond_wait(&finishCond, &finishMutex);
    pthread_mutex_unlock(&finishMutex);
    printf("Todas as threads terminaram\n");
}


void *barber() {
  
   while(1) {
      /* wait for a client to become available (sem_client) */
	  pthread_cond_wait(&client_,&chairs_mutex);
      /* wait for mutex to access chair count (chair_mutex) */
	  pthread_mutex_lock(&chairs_mutex);
      /* increment number of chairs available */
	  num_chairs += 1;
	  printf("Barber: Taking a client. Number of chairs available = %d\n",num_chairs);
      /* signal to client that barber is ready to cut their hair (sem_barber) */
	  pthread_cond_signal(&barber_);
      /* give up lock on chair count */
	  pthread_mutex_unlock(&chairs_mutex);

      /* cut hair for worktime seconds (really just call sleep()) */
     printf("Barber: cutting hair");
	
    } 
}

void *client() {

   while(1) {
      /* wait for mutex to access chair count (chair_mutex) */
	  pthread_mutex_lock(&chairs_mutex);
      /* if there are no chairs */
	  if(num_chairs <= 0){
           /* free mutex lock on chair count */
		   printf("Client: Thread %u leaving with no haircut\n", (unsigned int)pthread_self());
		   pthread_mutex_unlock(&chairs_mutex);
	  }
      /* else if there are chairs */
	  else{
           /* decrement number of chairs available */
		   num_chairs -= 1;
		   printf("Client: Thread %u Sitting to wait. Number of chairs left = %d\n",(unsigned int)pthread_self(),num_chairs);
           /* signal that a customer is ready (sem_client) */
		   pthread_cond_signal(&client_);
           /* free mutex lock on chair count */
		   pthread_mutex_unlock(&chairs_mutex);
           /* wait for barber (sem_barber) */
		   pthread_cond_wait(&barber_,&chairs_mutex);
           /* get haircut */
		   printf("Client: Thread %u getting a haircut\n",(unsigned int)pthread_self());
	  }
     
     printf("Client: Thread %u waiting seconds before attempting next haircut\n",(unsigned int)pthread_self());
     }

     thr_exit();
     return NULL;
}

int main() {

   pthread_t threadsall [MAX];

   pthread_create(&threadsall[0], NULL, barber, NULL);
   printf("Creating barber thread with id %lu\n",threadsall[0]);

   /* 4. Create client threads.  */
   for (int i = 0; i < MAX; i++){
	   pthread_create(&threadsall[i], NULL, client, NULL);
	   printf("Creating client thread with id %lu\n",threadsall[i]);
   }

   thr_join_all (MAX);
   return 0;
}


