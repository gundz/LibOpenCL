#include <OCL.h>
#include <unistd.h>
#include <stdio.h>

cl_int
openCLGetContext(cl_device_id *device_id, cl_context *context)
{
	cl_int			err;

	*context = clCreateContext(NULL, 1, device_id, NULL, NULL, &err);
	if (err != CL_SUCCESS)
		dprintf(STDERR_FILENO, "clCreateContext error: %d\n", err);
	return (err);
}