#include <pthread.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <list>
#include <unistd.h>

// define true como 1 e false como 0
#define  FALSE 0
#define  TRUE  1

// Declaracao das variaveis de condicao:
pthread_mutex_t  mutex;

//condições do buffer
#define  BUFFERVAZIO 0
#define  BUFFERCHEIO 1

int estado = BUFFERVAZIO;

//declara função primo
int primo(int n);

//estrutura para argumentos do produtor
typedef struct{
    int v;                    // numero de iterações do produtor
    std::list<int> *buffer;  // buffer utilizando a função list do c++
    int t;                  // tamanho maximo do buffer
}thread_produtor, *ptr_prod;
// estrutura nomeada thread_produtor e criado o ponteiro ptr_prod para ela


//estrutura para argumentos do consumidor
typedef struct{
    std::list<int> *buffer;  // buffer utilizando a função list do c++
    int t;                  //tamanho maximo do buffer
}thread_consumidor, *ptr_cons;
// estrutura nomeada thread_consumidor e criado o ponteiro ptr_cons para ela


// função que dita o comportamento da thread produtor
void *produtor(void *arg){
	ptr_prod argProd = (ptr_prod) arg;          // utiliza do ponteiro da struct para transformar os argumentos
	int i=0;                                    // contador
	int item;                                   // item produzido
    int aguardar;                               // condição
    unsigned long int id = pthread_self();      // guarda o id da thread

	printf("Inicio produtor %ld \n",id);
	while (i < argProd->v){                    // enquanto o contador for menor que o número de iterações
                                              //produzir item de valor aleatório
	    item = rand() % 1000;               
        do{
            pthread_mutex_lock(&mutex);         // loop enquanto thread tiver que esperar
	        aguardar = FALSE;                   // inicia sessão crítica e tira a thread do aguardo
	        if(argProd->buffer->size() == argProd->t){
	        	estado = BUFFERCHEIO;
	        }
	        if (estado == BUFFERCHEIO){         // se o buffer estiver cheio volta a aguardar, até que tenha espaço
		    	aguardar = TRUE;
	            pthread_mutex_unlock(&mutex);   // sai da sessão crítica
		    }
        }while (aguardar == TRUE);
		argProd->buffer->push_back(item);      //coloca o item no buffer
		printf("Produtor %ld inserindo item %d\n", id, item);	    //mostra seu id e o item
	    pthread_mutex_unlock(&mutex);               //sai da sessão critica
	    i++;
	    sleep(2);
	}
	printf("Produtor %ld terminado \n", id); 
}


// função que detalha comportamento da thread consumidor
void *consumidor(void *arg){
	ptr_cons argCons = (ptr_cons) arg;   //transforma argumentos
	int item;
	int aguardar;
	unsigned long int id = pthread_self();
	int pr; //auxpara calculo de primo

	printf("Inicio consumidor %ld \n",id);
	while (TRUE){   //loop infinito
        do{
            pthread_mutex_lock(&mutex);          // enquanto estiver aguardando tenta acessar a sessão critica
	        aguardar = FALSE;                    // fica aguardando se o buffer estiver vazio
	        if(argCons->buffer->empty()){
	        	aguardar = TRUE;
	        	pthread_mutex_unlock(&mutex);
	        }
        } while (aguardar == TRUE);
	    item = argCons->buffer->front();      //pega o primeiro item do buffer
      	argCons->buffer->pop_front();         // move o restante para frente (FIFO)
      	pthread_mutex_unlock(&mutex);
      	printf("Consumidor %ld terminado \n", id);
      	if(item == -1)
      		break;
      	pr = primo(item);                    //verifica se é primo
	    if(pr == 0){
		    printf("[%ld : %d]\n",id, item); // se for imprime o id da thread e o valor
	    }   
 	    sleep(2);
	}
}

int main(int argc, char *argv[]){
   	pthread_t * prod;  //cria threads de produtor e consumidor
	pthread_t * cons;
    int v=0, p=0, c=0, t=0;

    v = atoi(argv[1]);
    p = atoi(argv[2]);
    c = atoi(argv[3]);
    t = atoi(argv[4]);


    prod = (pthread_t*)malloc(p*sizeof(pthread_t));     // malloc para o número de threads produtor
    cons = (pthread_t*)malloc(c*sizeof(pthread_t));    // malloc para o número de threads consumidor
    thread_produtor argP[p];                          // Vetor com os argumentos a serem passados para produtores
    thread_consumidor argC[c];                       // Vetor com os argumentos a serem passados para consumidores
    std::list<int> buffer; 

    pthread_mutex_init(&mutex, NULL);                 // Inicialização do mutex

  
    for(int i=0; i<p; i++){ 
    	argP[i].v = v;
    	argP[i].buffer = &(buffer);
    	argP[i].t = t;
    	pthread_create(&(prod[i]), NULL, produtor, &(argP[i])); //criando as threads produtores passando os argumentos
    }


  	for(int i=0; i<c; i++){
    	argC[i].buffer = &(buffer);
    	argC[i].t = t;
    	pthread_create(&(cons[i]), NULL, consumidor, &(argC[i])); // cria as threads consumidores
  	}

 
  	for(int i=0; i<p; i++){
    	pthread_join(prod[i], NULL);        //espera a thread acabar
  	}

 	for(int i=0;i<c;i++){
    	buffer.push_back(-1);  //adiciona -1 no buffer na mesma quantidade de consumidores existentes
  	}

  	for(int i=0; i<c; i++){
    	pthread_join(cons[i], NULL);    //espera a thread acabar
  	}
  

  	printf("Terminado processo Produtor-Consumidor.\n\n");
  	return 0;
}

// função que verifica se determinado número é primo
int primo(int n)  {  
	int i, m=0, flag=0;   
  	m=n/2;  
  	for(i = 2; i <= m; i++){  
      	if(n % i == 0)  {    
          	flag=1;  
          	return 1;  // retorna 1 se não for
      	}  
  	}  
  	if (flag==0)  
      	return 0;    //retorna 0 se for
}  
