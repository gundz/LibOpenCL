#include <OCL.h>
#include <unistd.h>
#include <stdio.h>

cl_int
openCLBuildProgram(cl_program program)
{
	cl_int				err;

	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS)
		dprintf(STDERR_FILENO, "Error while building program\n");
	return (err);
}