#include <OCL.h>
#include <unistd.h>
#include <stdio.h>

cl_int
openCLGetProgram(cl_context context, const char **program_source, cl_program *program)
{
	cl_int			err;

	*program = clCreateProgramWithSource(context, 1, program_source, NULL, &err);
	if (err != CL_SUCCESS)
		dprintf(STDERR_FILENO, "clCreateProgramWithSource error: %d\n", err);
	return (err);
}