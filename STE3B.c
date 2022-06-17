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

#define N 10//Maxímo de lectores simultáneos.

//Contadores de escritores y lectores.
int escritores = 0, lectores = 0;

/*Semáforos 
ContadorLectores controla el acceso a contador de lectores.
semEscritores controla el acceso de escritores.
ContadorEscritores controla el acceso a contador de escritores, ver NOTA 3.*/
sem_t ContadorLectores, semEscritores, ContadorEscritores; 

//Función escritor
static void *escritor(void *ptr)
{
	
	int escritor = *(int *) ptr;//Numeración para identificar a cada escritor.

	printf("\nEscritor: %d, Escritores: %d, Lectores: %d, Estado: Ingreso", escritor, escritores, lectores);
	sem_wait(&ContadorEscritores); //Bloquea el acceso a la variable contador de escritores.
	escritores++;
	sem_post(&ContadorEscritores); //Libera el acceso a la variable contador de escritores.

	sem_wait(&semEscritores);//Bloquea a otros escritores.
	printf("\nEscritor: %d, Escritores: %d, Lectores: %d, Estado: Escribiendo", escritor, escritores, lectores);
		sleep(5);//escribe
	sem_post(&semEscritores);//Libera a otros escritores.

	sem_wait(&ContadorEscritores); //Bloquea el acceso a la variable contador de escritores.
	escritores--;
	sem_post(&ContadorEscritores); //Libera el acceso a la variable contador de escritores.
	printf("\nEscritor: %d, Escritores: %d, Lectores: %d, Estado: Salida", escritor, escritores, lectores);

	pthread_exit(0);
}

//Función lector.
static void *lector(void *ptr)
{
	int l = 1;//Para la salida del while
	
	int lector = *(int *) ptr;//Numeración para identificar a cada lector.
	
    while(l == 1)
    {
        
		sem_wait(&ContadorEscritores);//Bloquea el acceso a la variable contador de escritores, ver NOTA 1.
		sem_wait(&ContadorLectores); //Bloquea el acceso a la variable contador de lectores, ver NOTA 2.
		
        if(escritores == 0 && lectores < N) //Bloquea el acceso a lectores si hay escritores y más de N lectores.
        {
			printf("\nLector:   %d, Escritores: %d, Lectores: %d, Estado: Ingreso", lector, escritores, lectores);
            lectores++;
            if(lectores == 1) //Bloquea el acceso a los escritores si es el primer lector.
            {
                sem_wait(&semEscritores);
            }
			sem_post(&ContadorEscritores); //Libera el acceso a la variable contador de escritores, ver NOTA 1.
            sem_post(&ContadorLectores); //Libera el acceso a la variable contador de lectores.
            
			printf("\nLector:   %d, Escritores: %d, Lectores: %d, Estado: Leyendo", lector, escritores, lectores);
			sleep(5);//Lee.
			l = 0;//Habilito mi salida.
            
			sem_wait(&ContadorLectores); //Bloquea el acceso a la variable contador de lectores.
            lectores--;
			if(lectores == 0) //Libera el acceso a los escritores si es el ultimo lector.
            {
                sem_post(&semEscritores);
            }
            sem_post(&ContadorLectores); //Libera el acceso a la variable contador de lectores.
			printf("\nLector:   %d, Escritores: %d, Lectores: %d, Estado: Salida", lector, escritores, lectores);
        }else{
			sem_post(&ContadorEscritores); //Libera el acceso a la Variable contador de escritores, ver NOTA 1.
			sem_post(&ContadorLectores); //Libera el acceso a la variable contador de lectores, ver NOTA 2.
			printf("\nLector:   %d, Escritores: %d, Lectores: %d, Estado: Rechazado, duermo 5 segundos", lector, escritores, lectores);
			sleep(5);
		}
    }
	pthread_exit(0);
}
        
int main()
{
	int Nhilos = 20, numero[14] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
	
	//Inicio los semáforos.
	sem_init(&ContadorLectores, 0, 1);
	sem_init(&semEscritores, 0, 1);
	sem_init(&ContadorEscritores, 0, 1);
	
	//Creo los hilos.
	pthread_t hilos[Nhilos];
	
	//Entran 3 lectores.
	pthread_create(&hilos[0], NULL, lector, (void *)&numero[0]);
	sleep(1);
	pthread_create(&hilos[1], NULL, lector, (void *)&numero[1]);
	sleep(1);
	pthread_create(&hilos[2], NULL, lector, (void *)&numero[2]);
	sleep(1);
	
	//Entra 1 escritor.
	pthread_create(&hilos[3], NULL, escritor, (void *)&numero[0]);
	sleep(1);
	
	//Entra 1 lector.
	pthread_create(&hilos[4], NULL, lector, (void *)&numero[3]);
	sleep(1);
	
	//Entran 2 escritores.
	pthread_create(&hilos[5], NULL, escritor, (void *)&numero[1]);
	sleep(1);
	pthread_create(&hilos[6], NULL, escritor, (void *)&numero[2]);
	sleep(1);
	
	//Entran 10 lectores.
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
	
	//Entran 3 escritores.
	pthread_create(&hilos[17], NULL, escritor, (void *)&numero[3]);
	sleep(1);
	pthread_create(&hilos[18], NULL, escritor, (void *)&numero[4]);
	sleep(1);
	pthread_create(&hilos[19], NULL, escritor, (void *)&numero[5]);
	sleep(1);
	
	
	//Espera todos los hilos.
	int i;
	for(i=0;i<Nhilos;i++){
		pthread_join(hilos[i], NULL);
	}
	
	printf("\nFIN");
	
	return 0;
}



/*
A partir del comentario del acceso no protegido al contador
de lectores se detectaron algunos errores, los que se corri-
gen y explican a continuación.

NOTA 1:
Se podría dar el escenario de que ingresen escritores y lec-
tores simultáneamente cuando la base de datos está vacía, da-
ndo se una condición de carrera. En este escenario podrían in-
gresar más de un lector (posiblemente muchos) y por ende rom-
per la condición de prioridad a los escritores. Ya que los 
lectores podrían filtrarse entre los escritores en la instan-
cia del sem_wait del semáforo semEscritores.
Para evitar esto se restringe el acceso de lectura al conta-
dor de escritores antes del IF para que la consulta del con-
dicional sea limpia. Como esto deja bloqueado el acceso al 
semáforo del contador de escritores, se lo libera en las lí-
neas 70 y 86. Esto hace que la lectura sea atómica sin impor-
tar que camino del IF se siga, evitando que se produzcan in-
terbloqueos y mantiene la prioridad de los escritores.

NOTA 2:
Se podría dar el escenario que más de un lector lea el valor
del contador de lectores en N-1 y permitir el ingreso de más
lectores de los permitidos (este escenario fue el observado
en la presentación).
Para evitarlo, se adopta una política similar a la del esce-
nario de la nota 1, se restringe el acceso de lectura al con-
tador de lectores antes del IF para que la consulta del con-
dicional sea limpia. Se agrega el desbloqueo del semáforo en
la línea 87, ya que de otra manera quedaría bloqueado el se-
máforo si no se ingresa al true del IF, de por sí en el true
se desbloquea el semáforo luego de incrementar el contador 
de lectores. Esto hace que la lectura sea atómica sin impor-
tar que camino del IF se siga, evitando que se produzca des-
bordamiento de lectores.

NOTA 3:
Hay una corrección con esta nota respecto del código anterior,
se hizo un cambió de nombre de variables para que sea más fá-
cil seguir el código y no se actualizó esto. También hay un 
cambio en el nombre de algunas variables, ya que hubo un e-
rror al escribirlas y en esta versión se corrigieron.

*/
//gcc STE3B.c -o STE3B -pthread
//./STE3b