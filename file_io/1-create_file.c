#include "main.h"

/**
 * create_file - creates a file and writes text to it
 * @filename: the name of the file to create
 * @text_content: a NULL terminated string to write to the file
 *
 * Return: 1 on success, -1 on failure
 */
int create_file(const char *filename, char *text_content)
{
	int fd, i;
	ssize_t nwrite;

	if (filename == NULL)
		return (-1);

	fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
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
