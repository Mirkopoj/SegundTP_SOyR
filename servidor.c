
/* Estos son los ficheros de cabecera usuales */
#include <stdio.h>    
#include <stdlib.h>     
#include <strings.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/unistd.h>

#define PORT 3551 /* El puerto que será abierto */
#define BACKLOG 10 /* El número de conexiones permitidas */

int main()
{

   int fd, fd2; /* descriptores de sockets */
   char str[100];
   int done;

   struct sockaddr_in server; 
   /* para la información de la dirección del servidor */

   struct sockaddr_in client; 
   /* para la información de la dirección del cliente */

   int sin_size;

   /* A continuación la llamada a socket() */
   if ((fd=socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {  
      printf("error en socket()\n");
      exit(-1);
   }


   server.sin_family = AF_INET;         

   server.sin_port = htons(PORT); 
   /* Convierte de host a direccion de red corta  */

   server.sin_addr.s_addr = INADDR_ANY; 
   /* INADDR_ANY coloca nuestra dirección IP automáticamente */

   //memset(&(server.sin_zero), 0, sizeof(server.sin_zero));
   bzero(&(server.sin_zero),0); 
   /* escribimos ceros en el reto de la estructura */


   /* A continuación la llamada a bind() */
   if(bind(fd,(struct sockaddr*)&server, sizeof(struct sockaddr))==-1) {
      printf("error en bind() \n");
      exit(-1);
   }     

   
   if(listen(fd,BACKLOG) == -1) {  /* llamada a listen() */
      printf("error en listen()\n");
      exit(-1);
   }

   int i=0, n; 
   while(i < 10) {
      sin_size=sizeof(struct sockaddr_in);
       
      /* A continuación la llamada a accept() */
      if ((fd2 = accept(fd,(struct sockaddr *)&client, &sin_size))==-1) {
         printf("error en accept()\n");
         exit(-1);
      }

      char *desde = inet_ntoa(client.sin_addr);

      printf("Se obtuvo una conexión desde %s\n", desde ); 
      /* que mostrará la IP del cliente:  inet_ntoa() convierte a una cadena que contiene una dirección IP en un entero largo. */

      send(fd2,"Bienvenido a mi servidor.\n",22,0); 
      /* que enviará el mensaje de bienvenida al cliente */

      /******************/

       printf("Connected.\n");

        done = 0;
        do {
            n = recv(fd2, str, 100, 0);
            printf("Servidor: recibe %s \n", str);
            if (n <= 0) {
                if (n < 0) perror("recv");
                done = 1;
            }

            if (!done) 
                if (send(fd2, str, n, 0) < 0) {
                    perror("send");
                    done = 1;
                }
        } while (!done);

      //  close(s2);



      close(fd2); /* cierra fd2 */
      i++;
   }
}

