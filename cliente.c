#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <netdb.h>              
#include <pthread.h>
#include <unistd.h>

#define MAX_DATA 103   

void stress_test();
void *thread_cliente(void *a);

int main(int argc, char *argv[]){

	int sockid, numbytes,n, opc, PORT; //Fichostdataros descriptores y definiciones       
	char str[5];
	char buf[MAX_DATA]; //almacen de textos recibidos -> en BYTES  
	struct hostent *hostdata; //struct info sobre el host
	struct sockaddr_in servidor; //info direcciones del servidor para la conexión  

	//Indica si hubo error a la hora de corer el código
	if (argc !=2) { 
		printf("¡Oh no (⌣́_⌣̀)! Cometiste un error al usarlo, pero no te preocupes, te decimos como: %s <Dirección IP del SERVIDOR>\n",argv[0]);
		exit(-1);
   	}
	
	
   	//seleccionar el port dependiendo de que requieras:
   	// puerto 3552 es aleatorio, 3551 es secuencial
	printf(" ¿Así que querés recibir números primos /(•‿•)/ ?, Bueno \n");
	do{
		printf("Para recibirlos en un orden aleatorio inserte: 1\nDe lo contrario, si lo querés recibir en un orden secuencial inserte: 2\n");
		printf("Esta es la rama extrema de git, si queres llevar el servidor al limite presioná 3\n");
   		scanf("%d",&opc);
		printf("\n");
		switch (opc){
			case 1:
				PORT = 3552;
				break;
			case 2:
				PORT = 3551;
				break;
			case 3:
				stress_test(argv[1]);
				break;
			default:
				printf("CHE ヽ(ಠ_ಠ)ノ, te dije 1, 2 o 3... Porfa elegi uno válido.\n");
				opc = 0;
				break;
		}	
	}while(opc == 0);

	//llamada a gethostbyname() obtengo info del servidor a partir de su IP
   	if ((hostdata=gethostbyname(argv[1]))==NULL){       
      		printf("Error en la dirección IP\n");
      		exit(-1);
   	}

   	//llamada a socket(), AF_INET es el dominio (IPv4), SOCK_STREAM nos aseguramos conexión TCP
   	if ((sockid=socket(AF_INET, SOCK_STREAM, 0))==-1){  
      		printf("Error en llamar al socket\n");
      		exit(-1);
   	}
	
	//datos del servidor
	servidor.sin_family = AF_INET;
	servidor.sin_port = htons(PORT); 	
	servidor.sin_addr = *((struct in_addr *)hostdata->h_addr); //*hostdata->h_addr pasa la información de *hostdata a h_addr 
	bzero(&(servidor.sin_zero), sizeof(servidor.sin_zero));

	//llamada a connect(), para ppoder conectarse al servidor
	if(connect(sockid, (struct sockaddr *)&servidor, sizeof(struct sockaddr))==-1){ 
		printf("Error al conectar con el servidor\n");
      		exit(-1);
   	}
	
	//las siguientes dos funciones manejan la comunicación cliente-servidor
	
	//llamada a recv(), espera un mensaje del servidor
	if ((numbytes=recv(sockid,buf,MAX_DATA,0)) == -1){  
      		printf("Error al recibir \n");
      		exit(-1);
   	}

   	buf[numbytes]='\0';
   	printf("El servidor dice: %s\n",buf); 
	
	printf("Si en algun momento no sabes que hacer, estas peridido, desolado y necesitas ayuda (｡•́︿•̀｡), tipea 'help'.\n\n");
	//llamada a send(), envia un mensaje al servidor
     	while(strcmp(fgets(str, 5, stdin), "exit")) {
		if (strcmp(str, "help")==0){
			printf("Para recibir más numeros presioná [ENTER]\n");
			printf("Para finalizar la conexión tipea 'exit'\n\n");
			printf("Como te habrás dado cuenta, vos para poner 'help' tuviste que presionar [ENTER]...\n");
			printf("Así que,     ノ(˘⌣˘ノ), acá está tu número \n");
			continue;
		}
		if (send(sockid, "a", 1, 0) == -1) {
            		perror("send");
            		exit(1);
        	}
		if ((n=recv(sockid, str, 3, 0)) > 0) {
        	str[n] = '\0';
            	printf("Tú número es: %s \n", str);
        	} else {
			if (n < 0){
				perror("recv");
			} else {
				printf("¡De malas ლ(ಠ_ಠლ)!El servidor ha finalizado su conexión\n");
        		}
			close(sockid);
            		exit(-1);
    		}
		strcpy(str,"abcd");
	}
	//se cierra el socket
	close(sockid);   
}

void stress_test(void *ip){
	printf("\n\t¡¡¡¡¡ADVERTENCIA!!!!!\n");
	printf("\n\tEstas a punto de generar 40 threads");
	printf("\n\tCada uno va a generar un cliente que pida 1000 numeros");
	printf("\n\tEsto se va a repetir 1000 veces\n");
	printf("\n\tEstamos hablando de 40.000.000 de numeros pedidos\n");
	printf("Si estas seguro precioná [ENTER] para continuar\n");
	pthread_t clientes[40];
	for(int i=0;i<40;i++){
		pthread_create(&clientes[i], NULL, thread_cliente, ip);
	}
	for(int i=0;i<40;i++){
		pthread_join(clientes[i],0);
	}
	printf("\n\t40.000.000 de numeros primos mas tarde el server sigue en pie");
	exit(0);
}

void *thread_cliente(void * ip){
	int sockid, numbytes,n, opc, PORT; //Fichostdataros descriptores y definiciones       
	char str[5];
	char buf[MAX_DATA]; //almacen de textos recibidos -> en BYTES  
	struct hostent *hostdata; //struct info sobre el host
	struct sockaddr_in servidor; //info direcciones del servidor para la conexión  
	//llamada a gethostbyname() obtengo info del servidor a partir de su IP
   	if ((hostdata=gethostbyname((const char *)ip))==NULL){       
      		printf("Error en la dirección IP\n");
      		exit(-1);
   	}
	for (int locura=0;locura<1000;locura++) {
   	//llamada a socket(), AF_INET es el dominio (IPv4), SOCK_STREAM nos aseguramos conexión TCP
   	if ((sockid=socket(AF_INET, SOCK_STREAM, 0))==-1){  
      		printf("Error en llamar al socket\n");
      		exit(-1);
   	}
	
		//datos del servidor
		servidor.sin_family = AF_INET;
		PORT = 3551 + (rand()%2);
		servidor.sin_port = htons(PORT); 	
		servidor.sin_addr = *((struct in_addr *)hostdata->h_addr); //*hostdata->h_addr pasa la información de *hostdata a h_addr 
		bzero(&(servidor.sin_zero), sizeof(servidor.sin_zero));

			//llamada a connect(), para ppoder conectarse al servidor
			if(connect(sockid, (struct sockaddr *)&servidor, sizeof(struct sockaddr))==-1){ 
				printf("Error al conectar con el servidor\n");
		  		exit(-1);
			}
	
			//las siguientes dos funciones manejan la comunicación cliente-servidor
	
			//llamada a recv(), espera un mensaje del servidor
			if ((numbytes=recv(sockid,buf,MAX_DATA,0)) == -1){  
					printf("Error al recibir \n");
					exit(-1);
			}
	
			int muchas_veces = rand()%1000;
		  	for(int veces=0;veces<muchas_veces;veces++) {
			if (send(sockid, "a", 1, 0) == -1) {
				perror("send");
				exit(1);
			}
			if ((n=recv(sockid, str, 3, 0)) > 0) {
				str[n] = '\0';
				printf("%s ", str);
			} else {
				if (n < 0){
					perror("recv");
				} else {
					printf("¡De malas ლ(ಠ_ಠლ)!El servidor ha finalizado su conexión\n");
				}
				close(sockid);
			     exit(-1);
		 	}
		}
		//se cierra el socket
		close(sockid);
	}
	pthread_exit(0);
}



