/CLIENTE/
/***/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_TAM_MENSAJE 512 //Numero de caracteres maximo del mensaje

char mensaje[MAX_TAM_MENSAJE];

/********************/
/* función catch que captura una interrupción             */
/********************/
void catch(int sig)
{
	strcpy(mensaje,"terminar();");
}

/********************/
/* función MAIN                                           */
/* Orden Parametros: IP destino, puerto                   */
/********************/
int main(int argc, char *argv[]) {
	int puerto_id;
	struct sockaddr_in origen_dir, destino_dir;
	char respuesta[MAX_TAM_MENSAJE];//, mensaje[MAX_TAM_MENSAJE];

 	if (argc != 3) {
    printf("\n\nEl número de parámetros es incorrecto\n");
    printf("Use: %s <IP_servidor> <puerto>\n\n",argv[0]);
    exit(EXIT_FAILURE);
  }

	// Creamos el socket del cliente
	puerto_id = socket(AF_INET,SOCK_STREAM,0);
	if (puerto_id == -1) {
    printf("ERROR: El socket del cliente no se ha creado correctamente!\n");
    exit(EXIT_FAILURE);
  }

  //Se prepara la dirección de la máquina cliente
  origen_dir.sin_family = AF_INET;
	origen_dir.sin_port = htons(0); 								//Asigna un puerto disponible dela máquina
	origen_dir.sin_addr.s_addr = htonl(INADDR_ANY); //Asigna una IP de la máquina

	//Asigna una direccion local al socket
	if( bind(puerto_id, (struct sockaddr*)&origen_dir, sizeof(origen_dir)) == -1) {
    printf("ERROR al unir el socket a la dirección de la máquina cliente\n");
    close(puerto_id);
    exit(EXIT_FAILURE);
  }

  //Se prepara la dirección de la máquina servidora
	destino_dir.sin_family = AF_INET;
	destino_dir.sin_addr.s_addr = inet_addr(argv[1]);
	destino_dir.sin_port = htons(atoi(argv[2]));

	//Establece la conexión con la máquina remota
  printf("***Estableciendo conección con servidor %s en puerto %s ....\n",argv[1],argv[2]);
	if( connect(puerto_id, (struct sockaddr*)&destino_dir, sizeof(destino_dir)) == -1) {
    printf("ERROR en la solicitud de conección del cliente al servidor\n");
    close(puerto_id);
    exit(EXIT_FAILURE);
  } else {
    printf("***Conección establecida. \n");
  }
  signal(SIGINT, &catch);
  do {
    //Envía el mensaje
    printf("<<Client>>: ");
    scanf("%s",mensaje);
    if(send(puerto_id,mensaje, strlen(mensaje)+1, 0) == -1) {
      printf("ERROR al enviar el mensaje del cliente al servidor\n");
      close(puerto_id);
      exit(EXIT_FAILURE);
    } else {
      printf("***Cliente envía: %s a: %s en puerto: %s \n",mensaje, argv[1], argv[2]);
    }

	  //Recibe la respuesta
    if(recv(puerto_id, respuesta, sizeof(respuesta), 0) == -1) {
      printf("ERROR al recibir el mensaje del servidor al cliente\n");
      close(puerto_id);
      exit(EXIT_FAILURE);
    } else {
      printf("<<Server>>: Hola que tal\n", respuesta);
    }

    //Se cierra la conexión (socket)
    if(strcmp(mensaje,"terminar();") == 0) {
      printf("***Cliente terminó conección con servidor.\n");
      close(puerto_id);
      exit(EXIT_FAILURE);
    }

  } while(1);
	//Se cierra la conexión (socket)
  printf("\nCliente termina.\n");
	close(puerto_id);
  exit(EXIT_SUCCESS);
}
