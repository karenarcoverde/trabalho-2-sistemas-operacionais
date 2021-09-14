#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <list>

//MACROS - parâmetros que podem ser modificados pelo usuário
#define CLIENT_NUMBER 10 //número de clientes 
#define SIZE_FILA 5 //número de cadeiras, ou seja, número de pessoas que podem ficar na fila

using namespace std;

int clientePos = 0;
list<int> cadeiras; //lista utilizada para organizar os clientes em ordem

//estrutura para representar o cliente, cada cliente tem seu mutex e sua variável de condição
typedef struct {
   pthread_mutex_t mutex;
   pthread_cond_t  cond;
} cliente_struct;

//para o array cliente
pthread_mutex_t clientArray_mutex = PTHREAD_MUTEX_INITIALIZER;
cliente_struct clientes[CLIENT_NUMBER];

//para as cadeiras
pthread_mutex_t cadeiras_mutex = PTHREAD_MUTEX_INITIALIZER;

//para os clientes
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t client_ = PTHREAD_COND_INITIALIZER;

void *barber( void *) { 
   cliente_struct *currentCliente;

   //barbeiro está sempre cortando cabelo
   while(1) {
      //esperando por um cliente disponível
      pthread_cond_wait(&client_,&client_mutex);
      // pegando o primeiro cliente da fila - FIFO
      currentCliente = &clientes[cadeiras.front()];
      printf("Barber: atendendo cliente na posicao %i\n",cadeiras.front());
	  
      sleep(1); //espera um pouco para poder visualizar melhor
      //sinaliza para o cliente que o barbeiro está pronto para cortar o cabelo
	   pthread_cond_signal(&currentCliente->cond);

      pthread_mutex_lock(&cadeiras_mutex);
      printf("Barber: cabelo cortado \n");
      // elimina o primeiro elemento da lista de cadeiras, ou seja, vai para o próximo cliente que está na fila de cadeiras
	   cadeiras.pop_front();
      //como já cortou o cabelo e não precisa mais sentar na cadeira, libera o lock para outra thread entrar 
	   pthread_mutex_unlock(&cadeiras_mutex);
    } 
}

void *client(void *) {
   int posCopy;
   pthread_mutex_lock(&clientArray_mutex);
   posCopy = clientePos;
   clientes[posCopy].mutex = PTHREAD_MUTEX_INITIALIZER;
   clientes[posCopy].cond = PTHREAD_COND_INITIALIZER;
   clientePos++;
   pthread_mutex_unlock(&clientArray_mutex);
   
   //está sempre chegnado cliente, nunca para
   while(1) {
     //espera pelo mutex para acessar a fila de cadeiras
	  pthread_mutex_lock(&cadeiras_mutex);
     //se a fila estiver cheia e não tiver mais cadeira para sentar
	  if(cadeiras.size() >= SIZE_FILA){
		   printf("Client: Thread  %lu(%i) saindo sem corte de cabelo\n", pthread_self(),posCopy);
		   pthread_mutex_unlock(&cadeiras_mutex); //como nao conseguiu sentar na cadeira, libera o lock para outra thread entrar 
	  }
     //se tiver cadeira para sentar
	  else{
         
         cadeiras.push_back(posCopy);

		   printf("Client: Thread  %lu(%i) sentou para esperar. Numero de cadeiras ocupadas = %ld\n",pthread_self(),posCopy,cadeiras.size());
         pthread_mutex_unlock(&cadeiras_mutex);
         //sinaliza que o cliente está pronto
		   pthread_cond_signal(&client_);
         //espera pelo barbeiro
		   pthread_cond_wait(&clientes[posCopy].cond,&clientes[posCopy].mutex);
         //cabelo cortado
		   printf("Client: Thread  %lu(%i) cabelo cortado\n",pthread_self(),posCopy);
	  }
     sleep(1); //espera um pouco para poder visualizar melhor
     printf("Client: Thread %lu(%i) esperando segundos antes de tentar o proximo corte de cabelo\n",pthread_self(),posCopy);
   }
     return NULL;
}

int main() {
   pthread_t todasthreads [CLIENT_NUMBER+1]; //vetor com as threads do barbeiro e dos clientes. É somado 1 no índice para incluir o barbeiro
   //cria a thread para o barbeiro
   pthread_create(&todasthreads[0], NULL, barber, NULL);

   //cria as threads para os clientes
   for (int i = 1; i < CLIENT_NUMBER+1; i++){
	   pthread_create(&todasthreads[i], NULL, client, NULL);  
   }

   //para finalizar as threads
	for (int i = 0; i < CLIENT_NUMBER+1; i++){
			pthread_join(todasthreads[i],NULL);
	}
   return 0;
}

//Referências:
//1. Adaptado a partir do codigo fornecido pelo monitor https://github.com/tonyjmartinez/Sleeping-Barber/blob/ca168a12f539f24846a87950530bb7bed2206370/mybarber.c 
// Utilizando locks, variavéis de estado e FIFO
//2. Baseado na aula variavéis de condição: https://www.youtube.com/watch?v=a-WlBjCAP9Y&list=PL6i520yqwpsT-Zjj6jf9PA8M2oPk80ypf&index=22&ab_channel=RodrigodeSouzaCouto
//3. Fornecido pelo monitor: https://web.stanford.edu/~ouster/cgi-bin/cs140-spring14/lecture.php?topic=locks
//4. https://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_join.html
//5. https://www.cplusplus.com/reference/list/list/pop_front/
//6. https://www.cplusplus.com/reference/vector/vector/front/
//7. Código fornecido pelo professor: threadJoin.c