#include "buffer.h"

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define _BUFFER_MAX_SIZE 1000

struct buffer {
	int fd;
	char buffer[_BUFFER_MAX_SIZE];
	int pos;
	size_t len;
};

buffer *buff_create(int fd, size_t buffsz)
{
	struct buffer * new_buffer = malloc(sizeof(struct buffer));
	new_buffer->fd = fd;
	new_buffer->len = buffsz;
	new_buffer->pos = 0;

	return new_buffer;
}

int buff_getc(buffer *b)
{
	if (b == NULL){
		perror("NULL");
		exit(1);
	}

	//Vérifie si il n'y a plus rien à lire
	if(b->pos >= b->len){

		b->len = read(b->fd, b->buffer,_BUFFER_MAX_SIZE);
		if(b->len <=0) return EOF;

		b->pos = 0;
	}
	return b->buffer[b->pos++];
}

int buff_ungetc(buffer *b, int c)
{
	if (b == NULL){
		perror("NULL");
		exit(1);
	}

	//Vérifier que c n'est pas encore lu
	if(c >= b->pos){
		return -1;
	}

	b->pos = c;
	return c;
}

void buff_free(buffer *b)
{
	free(b);
	b = NULL;
}

int buff_eof(const buffer *buff)
{
	// Vérifie le EOF
	if(read(buff->fd, buff->buffer, 512) == 0){
		return 1;
	}
	return 0;
}

int buff_ready(const buffer *buff)
{
	// Le buff est encore lisible
	if(buff->pos < buff->len){
		return 1;
	}

	return 0;
}

char *buff_fgets(buffer *b, char *dest, size_t size)
{
    if (b == NULL || dest == NULL || size == 0) {
        return NULL;
    }

    int c;
    size_t i = 0;

    while (i < size - 1) {
        if (buff_eof(b)) break;

        c = buff_getc(b);
        if (c == '\n') {
            dest[i++] = c;
            break;
        }

        dest[i++] = c;
    }

    if (i == 0 && buff_eof(b)) {
        return NULL;  // rien lu, fin de fichier
    }

    dest[i] = '\0';
    return dest;
}


char *buff_fgets_crlf(buffer *b, char *dest, size_t size)
{
    if (b == NULL || dest == NULL || size == 0) {
        return NULL;
    }

    int c;
    size_t i = 0;

    while (i < size - 1) {
        if (buff_eof(b)) break;

        c = buff_getc(b);

        if (c == '\r') {
            int next = buff_getc(b);
            if (next == '\n') {
                // Reculer les deux
                buff_ungetc(b, b->pos - 2);
                dest[i++] = '\r';
                if (i < size - 1) dest[i++] = '\n';
                break;
            } else {
                buff_ungetc(b, b->pos - 1); // remet juste le \n si ce n’était pas \n
            }
        }

        dest[i++] = c;
    }

    if (i == 0 && buff_eof(b)) {
        return NULL;
    }

    dest[i] = '\0';
    return dest;
}

