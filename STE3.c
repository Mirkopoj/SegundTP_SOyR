#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

//Artilugio para que sleep corra en windows
#ifdef __unix__
# include <unistd.h>
#elif defined _WIN32
# include <windows.h>
#define sleep(x) Sleep(1000 * (x))
#endif

#define N 10//Lectores simultaneos maximo

//Contadores de escritores y lectores
int escritores = 0, lectores = 0;

/*Semaforos 
L controla el acceso a contador de lectores
semE controla el acceso de esritores
E controla el acceso a contador de escritores*/
sem_t ContadrLectores, semEscritores, ContadorEectores; 

//función escritor
static void *escritor(void *ptr)
{
	
	int escritor = *(int *) ptr;//Numeracion para identificar a acada escritor

	printf("\nEscritor: %d, Escritores: %d, Lectores: %d, Estado: Ingreso", escritor, escritores, lectores);
	sem_wait(&ContadorEectores); //bloquea el acceso a la variable contador de escritores
	escritores++;
	sem_post(&ContadorEectores); //libera el acceso a la variable contador de escritores

	sem_wait(&semEscritores);//bloquear otros escritores
	printf("\nEscritor: %d, Escritores: %d, Lectores: %d, Estado: Escribiendo", escritor, escritores, lectores);
		sleep(5);//escribe
		e = 0;//habilito mi salida
	sem_post(&semEscritores);//libera otros escritores

	sem_wait(&ContadorEectores); //bloquea el acceso a la variable contador de escritores
	escritores--;
	sem_post(&ContadorEectores); //libera el acceso a la contador de escritores
	printf("\nEscritor: %d, Escritores: %d, Lectores: %d, Estado: Salida", escritor, escritores, lectores);

	pthread_exit(0);
}

//función lector
static void *lector(void *ptr)
{
	int l = 1;//Para la salida del while
	
	int lector = *(int *) ptr;//Numeracion para identificar a acada lector
	
    while(l == 1)
    {
        
        if(escritores == 0 && lectores < N) //bloquea el acceso a lectores si hay escritores y mas de N lectores
        {
			printf("\nLector:   %d, Escritores: %d, Lectores: %d, Estado: Ingreso", lector, escritores, lectores);
			sem_wait(&ContadrLectores); //bloquea el acceso a la variable contador de lectores
            lectores++;
            if(lectores == 1) //bloquea el acceso a escritores si es el primer lector
            {
                sem_wait(&semEscritores);
            }
            sem_post(&ContadrLectores); //libera el acceso a la variable contador de lectores
            
			printf("\nLector:   %d, Escritores: %d, Lectores: %d, Estado: Leyendo", lector, escritores, lectores);
			sleep(5);//Lee
			l = 0;//habilito mi salida
            
			sem_wait(&ContadrLectores); //bloquea el acceso a la variable contador de lectores
            lectores--;
			if(lectores == 0) //libera el acceso a escritores si es el ultimo lector
            {
                sem_post(&semEscritores);
            }
            sem_post(&ContadrLectores); //libera el acceso a la variable contador de lectores
			printf("\nLector:   %d, Escritores: %d, Lectores: %d, Estado: Salida", lector, escritores, lectores);
        }else{
			printf("\nLector:   %d, Escritores: %d, Lectores: %d, Estado: Rechazado, duermo 5 segundos", lector, escritores, lectores);
			sleep(5);
		}
    }
	pthread_exit(0);
}
        
int main()
{
	int Nhilos = 20, numero[14] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
	
	//inicio los semaforos
	sem_init(&ContadrLectores, 0, 1);
	sem_init(&semEscritores, 0, 1);
	sem_init(&EContadorEectores, 0, 1);
	
	//creo los hilos
	pthread_t hilos[Nhilos];
	
	//entran 3 lectores
	pthread_create(&hilos[0], NULL, lector, (void *)&numero[0]);
	sleep(1);
	pthread_create(&hilos[1], NULL, lector, (void *)&numero[1]);
	sleep(1);
	pthread_create(&hilos[2], NULL, lector, (void *)&numero[2]);
	sleep(1);
	
	//entra 1 escritor
	pthread_create(&hilos[3], NULL, escritor, (void *)&numero[0]);
	sleep(1);
	
	//entra 1 lector
	pthread_create(&hilos[4], NULL, lector, (void *)&numero[3]);
	sleep(1);
	
	//entran 2 escritor
	pthread_create(&hilos[5], NULL, escritor, (void *)&numero[1]);
	sleep(1);
	pthread_create(&hilos[6], NULL, escritor, (void *)&numero[2]);
	sleep(1);
	
	//entran 10 lectores
	pthread_create(&hilos[7], NULL, lector, (void *)&numero[4]);
	sleep(1);
	pthread_create(&hilos[8], NULL, lector, (void *)&numero[5]);
	sleep(1);
	pthread_create(&hilos[9], NULL, lector, (void *)&numero[6]);
	sleep(1);
	pthread_create(&hilos[10], NULL, lector, (void *)&numero[7]);
	sleep(1);
	pthread_create(&hilos[11], NULL, lector, (void *)&numero[8]);
	sleep(1);
	pthread_create(&hilos[12], NULL, lector, (void *)&numero[9]);
	sleep(1);
	pthread_create(&hilos[13], NULL, lector, (void *)&numero[10]);
	sleep(1);
	pthread_create(&hilos[14], NULL, lector, (void *)&numero[11]);
	sleep(1);
	pthread_create(&hilos[15], NULL, lector, (void *)&numero[12]);
	sleep(1);
	pthread_create(&hilos[16], NULL, lector, (void *)&numero[13]);
	sleep(1);
	
	//entra 3 escritores
	pthread_create(&hilos[17], NULL, escritor, (void *)&numero[3]);
	sleep(1);
	pthread_create(&hilos[18], NULL, escritor, (void *)&numero[4]);
	sleep(1);
	pthread_create(&hilos[19], NULL, escritor, (void *)&numero[5]);
	sleep(1);
	
	
	//Espera todos los hilos
	for(int i=0;i<h;i++){
		pthread_join(hilos[i], NULL);
	}
	
	printf("\nFIN");
	
	return 0;
}
