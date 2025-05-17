#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list/list.h"
#include "user.h"

#define PORT_FREESCORD 4321

#define MAXSIZE 256
#define MAXUSER 10

/** Gérer toutes les communications avec le client renseigné dans
 * user, qui doit être l'adresse d'une struct user */
void *handle_client(void *user);
/** Créer et configurer une socket d'écoute sur le port donné en argument
 * retourne le descripteur de cette socket, ou -1 en cas d'erreur */
int create_listening_sock(uint16_t port);

int main(int argc, char *argv[])
{
	printf("Démarrage du serveur...\n");
	
	int server = create_listening_sock(PORT_FREESCORD);
	
	while(1){
		// Ajout de user
		struct user * user_1 = user_accept(server);
		if(user_1 == NULL){
			perror("Erreur de création d'user...");
			exit(1);
		}

		pthread_t per_user;
		if (pthread_create(&per_user,NULL,handle_client,user_1) != 0){
			perror("pthread_create");
			close(user_1->sock);
			user_free(user_1);
			continue;
		}

		pthread_detach(per_user);

	}

	close(server);
	return 0;
}

void *handle_client(void *clt)
{
	printf("New user accepted...\n");
	struct user * user = (struct user * ) clt;
	
	char buffer[MAXSIZE];
	while(1){
		//Attente de lecture
		size_t n = read(user->sock, buffer, MAXSIZE);
		
		if (n <= 0){
			printf("\nClient déconnecter...\n");
			break;;
		} else {
			printf("Message client : %s\n", buffer);
			send(user->sock, buffer, strlen(buffer), 0);
		}
	}

	close(user->sock);
	user_free(clt);

	//Non used variable
	return clt;
}

int create_listening_sock(uint16_t port)
{
	//Creation du socket
	int sock_1 = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_1 < 0)
	{
		perror("socket");
		exit(1);
	} else {
		printf("\nSock créer...\n");
	}

	/*Création du structure adresse*/
	struct sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		sa.sin_addr.s_addr = htonl(INADDR_ANY);

	socklen_t sl = sizeof(sa);

	int opt = 1;
	if(setsockopt(sock_1,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0){
		perror("setsockopt");
		return -1;
	}
	
	if(bind(sock_1,(struct sockaddr * ) &sa,sl) < 0){
		perror("bind");
		return -1;
	} else {
		printf("Binding success...\n");
	}

	if(listen(sock_1,128) < 0){
		perror("listen");
		return -1;
	} else {
		printf("Listen success...\n");
	}

	return sock_1;
}
