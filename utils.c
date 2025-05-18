#include <string.h>
char *crlf_to_lf(char *line_with_crlf)
{
	size_t length = strlen(line_with_crlf);
	if (length <= 510 && line_with_crlf[length] == '\r'){
		line_with_crlf[length] = '\n';
		line_with_crlf[length+1] = '\0';
	} else {
		return NULL;
	}

	/* pour éviter les warnings de variable non utilisée */
	return line_with_crlf;
}

char *lf_to_crlf(char *line_with_lf)
{
	size_t length = strlen(line_with_lf);
	if (length <= 510 && line_with_lf[length] == '\n'){
		line_with_lf[length-1] = '\r';
		line_with_lf[length] = '\n';
		line_with_lf[length+1] = '\0';
	} else {
		return NULL;
	}

	/* pour éviter les warnings de variable non utilisée */
	return line_with_lf;
}
