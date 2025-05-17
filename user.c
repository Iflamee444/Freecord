#include "user.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>

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
