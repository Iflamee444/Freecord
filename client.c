#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include "buffer/buffer.h"
#include "utils.h"
#include <string.h>

#define PORT_FREESCORD 4321
#define MAXSIZE 256

/** se connecter au serveur TCP d'adresse donnée en argument sous forme de
 * chaîne de caractère et au port donné en argument
 * retourne le descripteur de fichier de la socket obtenue ou -1 en cas
 * d'erreur. */
int connect_serveur_tcp(char *adresse, uint16_t port);

int main(int argc, char *argv[])
{
	//Argument counter
	if (argc != 2){
		fprintf(stderr, "Usage : %s <adresse>\n", argv[0]);
		return 1;
	}

	int socket = connect_serveur_tcp(argv[1],PORT_FREESCORD);
	if (socket < 0) return -1;

	//VARIABLES
	char input[MAXSIZE];

	while (1)
	{
		// Reception du message
		printf("Entrer un message : ");
		if(fgets(input, sizeof(input),stdin) == NULL) {
			printf("\nDéconnection du serveur...\n");
			break;
		}

		//Envoi du message
		send(socket, input, sizeof(input), 0);

		int reader = read(socket, input, sizeof(input) - 1) ;
		if((reader <= 0)){
			fprintf(stderr, "Erreur de réception");
			break;
		}
		input[reader] = '\0';
		printf("Réponse du serveur : %s\n", input);
		
	}
	
	close(socket);
	return 0;
}

int connect_serveur_tcp(char *adresse, uint16_t port)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0){
		perror("Erreur lors de la creation du socket");
		return -1;
	}

	struct sockaddr_in sa = {
		.sin_family = AF_INET,
		.sin_port = htons(port)};
	if(inet_pton(AF_INET, adresse, &sa.sin_addr) != 1){
		perror("inet");
		return -1;
	}

	socklen_t sl = sizeof(sa);

	if(connect(fd, (struct sockaddr *) &sa, sl) < 0){
		perror("connect");
		return -1;
	}

	return fd;

	/* pour éviter les warnings de variable non utilisée */
	//return *adresse + port;
}
