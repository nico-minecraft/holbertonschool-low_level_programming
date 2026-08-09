#include "main.h"

/**
 * append_text_to_file - appends text at the end of a file
 * @filename: the name of the file
 * @text_content: a NULL terminated string to add to the end of the file
 *
 * Return: 1 on success, -1 on failure
 */
int append_text_to_file(const char *filename, char *text_content)
{
	int fd, i;
	ssize_t nwrite;

	if (filename == NULL)
		return (-1);

	fd = open(filename, O_WRONLY | O_APPEND);
	if (fd == -1)
		return (-1);

	if (text_content != NULL)
	{
		for (i = 0; text_content[i] != '\0'; i++)
			;

		nwrite = write(fd, text_content, i);
		if (nwrite == -1 || nwrite != i)
		{
			close(fd);
			return (-1);
		}
	}

	close(fd);
	return (1);
}
