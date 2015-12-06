#include <OCL.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

cl_int
openCLGetProgramFromFile(cl_context context, const char *path, cl_program *program)
{
	struct stat			buf;
	int					fd;
	char				*program_source;
	cl_int				err;

	if ((fd = open(path, O_RDONLY)) == -1)
	{
		dprintf(STDERR_FILENO, "No such file or directory: %s\n", path);
		return (-1);
	}
	if (stat(path, &buf) == -1)
	{
		close(fd);
		return (-1);
	}
	if (!(program_source = (char *)malloc(sizeof(char) * (buf.st_size + 1))))
	{
		close(fd);
		return (-1);
	}
	if ((err = read(fd, program_source, buf.st_size)) != -1)
	{
		program_source[buf.st_size] = '\0';
		err = openCLGetProgram(context, (const char **)&program_source, program);
	}
	free(program_source);
	close(fd);
	return (err);
}