#include "user.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>


static list_user *head = NULL;
static pthread_mutex_t users_mutex = PTHREAD_MUTEX_INITIALIZER;

/** accepter une connection TCP depuis la socket d'écoute sl et retourner un
 * pointeur vers un struct user, dynamiquement alloué et convenablement
 * initialisé */
struct user *user_accept(int sl)
{

	struct user *new_user = malloc(sizeof(struct user));
	new_user->addr_len = sizeof(struct sockaddr_in);
	new_user->address = malloc(new_user->addr_len);
	new_user->sock = accept(sl, new_user->address, &new_user->addr_len);
	if(new_user->sock < 0){
		perror("accept");
		exit(1);
	} else {
		printf("Accepted...\n");
	}

	/* pour éviter les warnings de variable non utilisée */
	//sl = 0;
	return new_user;
}

/** libérer toute la mémoire associée à user */
void user_free(struct user *user)
{
	 if (user) {
        free(user->address);  // Free address memory
        free(user);           // Free user struct
    }

	/* pour éviter les warnings de variable non utilisée */
	user = NULL;
}


void init_user(){
	head = NULL;
	pthread_mutex_init(&users_mutex, NULL);
}


void add_user(struct user *user){

	list_user *chaine = malloc(sizeof(list_user));
	if(!chaine){
		perror("malloc");
		return;
	}

	chaine->user = user;
	chaine->next = NULL;

	pthread_mutex_lock(&users_mutex);
	chaine->next = head;
	head = chaine;
	pthread_mutex_unlock(&users_mutex);

	printf("Nouvel utilisateur : %d ajouter...\n", user->sock);
	fflush(stdout);
}

void remove_user(int sock){
	pthread_mutex_lock(&users_mutex);
	list_user **current = &head;
	while(*current){
		if((*current)->user->sock == sock){
			list_user *to_free = *current;
			*current = to_free->next;

			close(to_free->user->sock);
			user_free(to_free->user);
			break;
		} else {
			current = &(*current)->next;
		}
	}

	printf("Utilisateur : %d retirer...\n", sock);
	fflush(stdout);
}

void user_messages(struct user *sender, const char *msg, ssize_t len){
	pthread_mutex_lock(&users_mutex);
	list_user *current = head;
	while (current)
	{
		if(current->user != sender){
			send(current->user->sock, msg, len, 0);
		}
		current = current->next;
	}
	
	pthread_mutex_unlock(&users_mutex);
}

void user_broadcast(const char *msg, ssize_t len){
	pthread_mutex_lock(&users_mutex);
	list_user *current = head;
	while (current)
	{
		send(current->user->sock, msg, len, 0);
		current = current->next;
	}
	
	pthread_mutex_unlock(&users_mutex);
}