/* Estos son los ficheros de cabecera usuales */
#include <strings.h> 
#include <sys/poll.h>
#include <arpa/inet.h>
#include <sys/unistd.h>
#include <semaphore.h>

#include <future>
#include <unistd.h>

#define PORT_SEC 3551  /* El puerto para numeros secuenciales */
#define PORT_RAND 3552 /* El puerto para numeros aleatorios */
#define BACKLOG 10 /* El número de conexiones permitidas */

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

enum MODO{
	apagado,
	secuencial,
	aleatrorio
};

typedef struct client_fifo_struct{
	int tad[BACKLOG];
	int entrada;
	int salida;
	sem_t sem_fifo;
} client_fifo_t;

void client_thread(int fd2, enum MODO *modo, sem_t *anti_backlog, client_fifo_t *client_fifo, int mi_slot){
   char str[100];
	int n, errsv;
	unsigned char mensaje, done = 0, cont = 0xFF;

	while (!done) {		
		mensaje = (*modo == secuencial)? (cont++):rand();
		if (send(fd2, primos[mensaje%25], 2, 0) < 0) {
			perror("send");
			done = 1;
		}

		printf("prewhile2\n");

		n=0;
		while(*modo != apagado && n < 1 && !done){
			n = recv(fd2, str, 100, MSG_DONTWAIT);	
			errsv = errno;
			if (n == 0) done = 1;
			if (n < 0){
				if((errsv!=EAGAIN)&&(errsv!=EWOULDBLOCK)){
					perror("recv");
					done = 1;
				}
			}
		}

		if(*modo == apagado) done = 1;
		printf("postwhile2, modo: %d, apagado: %d, n: %d, done: %d\n", *modo, apagado, n, done);
	};

	close(fd2); 
	sem_wait(&(client_fifo->sem_fifo));
	client_fifo->tad[client_fifo->entrada] = mi_slot;
	client_fifo->entrada = (client_fifo->entrada+1)%BACKLOG;
	sem_post(&(client_fifo->sem_fifo));
	sem_post(anti_backlog);
	return;
}

void server_thread(enum MODO *modo){
   int fd, fd2; /* descriptores de sockets */

   struct sockaddr_in server; 
   /* para la información de la dirección del servidor */

   struct sockaddr_in client; 
   /* para la información de la dirección del cliente */

   unsigned int sin_size;

   /* A continuación la llamada a socket() */
   if ((fd=socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {  
      printf("error en socket()\n");
      exit(-1);
   }


   server.sin_family = AF_INET;         

	uint16_t puerto = (*modo == secuencial)? PORT_SEC:PORT_RAND;
   server.sin_port = htons(puerto); 
   /* Convierte de host a direccion de red corta  */

   server.sin_addr.s_addr = INADDR_ANY; 
   /* INADDR_ANY coloca nuestra dirección IP automáticamente */

   //memset(&(server.sin_zero), 0, sizeof(server.sin_zero));
   bzero(&(server.sin_zero), sizeof(server.sin_zero)); 
   /* escribimos ceros en el reto de la estructura */


   /* A continuación la llamada a bind() */
   if(bind(fd,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1) {
      printf("error en bind() \n");
      exit(-1);
   }     

   if(listen(fd,BACKLOG) == -1) {  /* llamada a listen() */
      printf("Error en listen(): %d\n", *modo);
      exit(-1);
   }

   int i; 
	unsigned char cont = 0;
   char *ip_cliente;

	int poll_status;
	struct pollfd poll_str = {
		.fd = fd,
		.events = POLLIN,
	};

	sem_t anti_backlog;

	client_fifo_t client_fifo;
	sem_init(&client_fifo.sem_fifo, 0, 0);
	client_fifo.entrada = 0;
	client_fifo.salida = 0;
	for(i=0;i<BACKLOG;i++){
		client_fifo.tad[i] = i;	
	}
	i=0;
	int client_slot;

	sem_init(&anti_backlog, 0, BACKLOG);
	std::future<void> clientes[BACKLOG];
   
	while(*modo != apagado) {
      sin_size=sizeof(struct sockaddr_in);
       
      /* A continuación la llamada a accept() */
		printf("prewhile1\n");

		do {
			poll_status = poll(&poll_str, 1, 1000);
		} while(*modo != apagado && poll_status < 1);

		printf("postwhile1\n");

		if(poll_status < 0){
			printf("Error en poll(): %d\n", *modo);
			exit(-1);
		}

		if(*modo == apagado){
			printf("imaheadout\n");
      
			close(fd2); /* cierra fd2 */
			close(fd);
			printf("yending\n");

			return;
			printf("ido\n");
		}

		fd2 = accept(fd, (struct sockaddr *) &client, &sin_size);

      ip_cliente = inet_ntoa(client.sin_addr);

      printf("Se obtuvo una conexión desde %s\n", ip_cliente); 
      /* que mostrará la IP del cliente:  inet_ntoa() convierte a una cadena que contiene una dirección IP en un entero largo. */

      send(fd2,"*BIENVENIDO AL FANTASTICO GENERADOR DE NUMEROS PRIMOS*\n(puramente matematico no hay una tabla detras)\n",102,0); 
      /* que enviará el mensaje de bienvenida al cliente */

      /******************/

		printf("Connected.\n");

		int sval;
		sem_wait(&anti_backlog);

		sem_wait(&client_fifo.sem_fifo);
		client_slot = client_fifo.tad[client_fifo.salida];
		client_fifo.salida = (client_fifo.salida+1)%BACKLOG;
		sem_post(&client_fifo.sem_fifo);

		clientes[client_slot] = std::async(std::launch::async, client_thread, fd2, modo, &anti_backlog, &client_fifo, client_slot);
      

   }
	close(fd2);
	close(fd);

	return;
}

int main(){

	std::future<void> server_sec_thread, server_rand_thread;
	enum MODO modo_sec = secuencial, modo_rand = aleatrorio;

	server_sec_thread  = std::async(std::launch::async, server_thread, &modo_sec);
	server_rand_thread = std::async(std::launch::async, server_thread, &modo_rand);

	printf("El servidor esta en ejecucion, presione cualquier tecla para finalizarlo\n");
	getchar();
	modo_sec = apagado;
	modo_rand = apagado;

	printf("a esperar\n");
	server_sec_thread.wait();
	printf("no sec\n");
	server_rand_thread.wait();

	printf("Se acabaron los numeros primos");

	return 0;
}
