#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <netdb.h>              

#define MAX_DATA 103   

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
		printf("Para recibirlos en un orden aleatorio inserte: 1 \n De lo contrario, si lo querés recibir en un orden secuencial inserte: 2\n");
   		scanf("%d",&opc);
		printf("\n");
		switch (opc){
			case 1:
				PORT = 3552;
				break;
			case 2:
				PORT = 3551;
				break;
			default:
				printf("CHE ヽ(ಠ_ಠ)ノ, te dije 1 o 2... Porfa elegi uno válido.\n");
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
