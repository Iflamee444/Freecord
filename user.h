#ifndef USER_H
#define USER_H
#include <sys/socket.h>
#include <netinet/in.h>

struct user {
	struct sockaddr *address;
	socklen_t addr_len;
	int sock;
	/* autres champs éventuels */
};

typedef struct list_user {
	struct user * user;
	struct list_user * next;
	
} list_user;

//Fonction de gestion d'user
void init_user();
void add_user(struct user *user);
void remove_user(int sock);

//Liaison entre toutes les utilisateurs
void user_messages(struct user *sender, const char *msg, ssize_t len);
void user_broadcast(const char *msg, ssize_t len);

//Utilisation des sockets
struct user *user_accept(int sl);
void user_free(struct user *user);

#endif /* ifndef USER_H */
