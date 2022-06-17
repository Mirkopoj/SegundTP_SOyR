#include <cstdio>
#include <strings.h> 
#include <sys/poll.h>
#include <arpa/inet.h>
#include <sys/unistd.h>
#include <semaphore.h>
#include <future>
#include <unistd.h>

#define PORT_SEC 3551  /* El puerto para numeros secuenciales */
#define PORT_RAND 3552 /* El puerto para numeros aleatorios */
#define BACKLOG 10	  /* El maximo número de clientes simultaneos */

/*"Generador de numeros primos"*/
const char *primos[25] = {
     " 2"," 3"," 5"," 7",
"11",     "13",     "17","19",
          "23",          "29",
"31",               "37",
"41",     "43",     "47",
          "53",          "59",
"61",               "67",
"71",     "73",          "79",
          "83",          "89",
	                      "97"
};

/*Modos de operacion de los threads*/
enum MODO{
	apagado,
	secuencial,
	aleatrorio
};

/*Estructura fifo para coordinar los futuros de cada cliente*/
typedef struct client_fifo_struct{
	int arr[BACKLOG];
	int entrada;
	int salida;
	sem_t sem_fifo;
} client_fifo_t;

/*Funcion para atender a un cliente*/
void client_thread(int client_socket, enum MODO *modo, sem_t *anti_backlog, client_fifo_t *client_fifo, int mi_slot);

/*Funcion para operar el servidor en cada puerto*/
void server_thread(enum MODO *modo);

int main(){

	/*future es equivalente al pthread_t de C, pero permite guardar un posible return del thread*/
	std::future<void> server_sec_thread, server_rand_thread;
	enum MODO modo_sec = secuencial, modo_rand = aleatrorio;

	/*Se lanzan los threads para cada puerto*/
	server_sec_thread  = std::async(std::launch::async, server_thread, &modo_sec);
	server_rand_thread = std::async(std::launch::async, server_thread, &modo_rand);

	printf("El servidor esta en ejecucion, presione [ENTER] para finalizarlo\n");
	getchar();
	modo_sec = apagado;
	modo_rand = apagado;

	/*Equivalente al join() de C*/
	server_sec_thread.wait();
	server_rand_thread.wait();

	printf("Se acabaron los numeros primos ◉╭╮◉\n");

	return 0;
}

void client_thread(int client_socket, enum MODO *modo, sem_t *anti_backlog, client_fifo_t *client_fifo, int mi_slot){
		/* Envia una calida bienvenida al cliente */
      send(client_socket,"*BIENVENIDO AL FANTASTICO GENERADOR DE NUMEROS PRIMOS*\n(puramente matematico no hay una tabla detras)\n",102,0); 

   char *str;
	int n, errsv;
	unsigned char mensaje, client_stop = 0, cont = 0;

	/* Loop principal de atencion al cliente */
	while (!client_stop) {		

		/* Se envia un numero primo */
		mensaje = (*modo == secuencial)? (cont++):rand();
		if (send(client_socket, primos[mensaje%25], 2, 0) < 0) {
			perror("send");
			client_stop = 1;
		} 

		/* Se espera una respuesta del cliente de forma no bloqueante de modo que se pueda abortar el server */
		n=0;
		while(*modo != apagado && n < 1 && !client_stop){
			n = recv(client_socket, str, 1, MSG_DONTWAIT);	
			errsv = errno;
			if (n == 0) client_stop = 1;
			if (n < 0){
				if((errsv!=EAGAIN)&&(errsv!=EWOULDBLOCK)){
					perror("recv");
					client_stop = 1;
				}
			}
		}

		/* En case de que se abortara el server se finaliza el cliente */
		if(*modo == apagado) client_stop = 1;
	}
	/* Fin del loop principal de atencion al cliente */

	close(client_socket); 

	/* Devulve el indice que ocupaba en el array de futuros a la FIFO */
	sem_wait(&(client_fifo->sem_fifo));
	client_fifo->arr[client_fifo->entrada] = mi_slot;
	client_fifo->entrada = (client_fifo->entrada+1)%BACKLOG;
	sem_post(&(client_fifo->sem_fifo));

	sem_post(anti_backlog);

	return;
}

void server_thread(enum MODO *modo){
   int server_socket, client_socket; 

   struct sockaddr_in server; 
   struct sockaddr_in client; 
   unsigned int sin_size;

   /* Se crea el server_socket*/
   if ((server_socket=socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {  
      printf("error en socket()\n");
      exit(-1);
   }

   server.sin_family = AF_INET;         

	/* Se asigna un puerto segun el modo */
	uint16_t puerto = (*modo == secuencial)? PORT_SEC:PORT_RAND;
   server.sin_port = htons(puerto); 

	/* Se guarda la ip del server*/
   server.sin_addr.s_addr = INADDR_ANY; 

   bzero(&(server.sin_zero), sizeof(server.sin_zero)); 

	/* Se asocia el puerto asignado al socket */
   if(bind(server_socket,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1) {
      printf("error en bind() \n");
      exit(-1);
   }     

	/* "Publica" el server_socket */
   if(listen(server_socket,BACKLOG) == -1) { 
      printf("Error en listen(): %d\n", *modo);
      exit(-1);
   }

   int i; 
	unsigned char cont = 0;
   char *ip_cliente;

	/* Estructura de poll se utilizará en accept*/
	int poll_status;
	struct pollfd poll_str = {
		.fd = server_socket,
		.events = POLLIN,
	};

	sem_t anti_backlog;

	/* FIFO de direcciones en un array de futuros para el lanzamiento de los threads de clientes */
	client_fifo_t client_fifo;
	sem_init(&client_fifo.sem_fifo, 0, 1);
	client_fifo.entrada = 0;
	client_fifo.salida = 0;
	for(i=0;i<BACKLOG;i++) client_fifo.arr[i] = i;	
	i=0;
	int client_slot;

	/* Array de futuros y semaforo para impedir que se atiendan mas clientes de lo que el BACKLOG permite */
	sem_init(&anti_backlog, 0, BACKLOG);
	std::future<void> clientes[BACKLOG];

	/* Para un printf */
	int puertos[3] = {
		-1,
		PORT_SEC,
		PORT_RAND,
	};
   
	int contador_clientes;
	/* Loop principal del server */
	while(*modo != apagado) {
      sin_size=sizeof(struct sockaddr_in);
       
      /* Se espera a un cliente disponible o a que se aborte el server */
		poll_status = 0;
		while(*modo != apagado && poll_status == 0) poll_status = poll(&poll_str, 1, 1000);

		if(poll_status < 0){
			printf("Error en poll(): %d\n", *modo);
			exit(-1);
		}

		if(*modo == apagado) break;

		/* Recive al cliente */
		client_socket = accept(server_socket, (struct sockaddr *) &client, &sin_size);

      ip_cliente = inet_ntoa(client.sin_addr);

		if(sem_getvalue(&anti_backlog, &contador_clientes)!=0){
			contador_clientes = BACKLOG+1;
		}
      printf("%s, se conectó en el puerto: %d. Clientes conectados: %d\n", ip_cliente, puertos[*modo], BACKLOG-contador_clientes); 


		/* No procesa mas clientes en simultaneo de lo que el BACKLOG permite */
		sem_wait(&anti_backlog);

		/*Busca en el FIFO de direcciones a que futuro asignar el thread del cliente */
		sem_wait(&client_fifo.sem_fifo);
		client_slot = client_fifo.arr[client_fifo.salida];
		client_fifo.salida = (client_fifo.salida+1)%BACKLOG;
		sem_post(&client_fifo.sem_fifo);

		/* Lanza un nuevo thread para el cliente entrante */
		clientes[client_slot] = std::async(std::launch::async, client_thread, client_socket, modo, &anti_backlog, &client_fifo, client_slot);
      

   }
	/* Fin del loop principal del server */

	close(client_socket);
	close(server_socket);

	return;
}
