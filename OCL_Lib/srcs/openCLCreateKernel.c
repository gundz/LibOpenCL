#include <OCL.h>
#include <unistd.h>
#include <stdio.h>

cl_int
openCLCreateKernel(cl_program program, const char *kernel_name, cl_kernel *kernel)
{
	cl_int				err;

	*kernel = clCreateKernel(program, kernel_name, &err);
	if (err != CL_SUCCESS)
		dprintf(STDERR_FILENO, "Error while creating kernel: %d\n", err);
	return (err);
}