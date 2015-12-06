#include <OCL.h>
#include <unistd.h>
#include <stdio.h>

cl_int
openCLGetCommandQueue(cl_context context, cl_device_id device_id, cl_command_queue *command_queue)
{
	cl_int			err;

	#ifndef __LINUX__
	// *command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, &err);
	// #elif __APPLE__
	*command_queue = clCreateCommandQueue(context,  device_id, 0, &err);
	#endif

	if (err != CL_SUCCESS)
		dprintf(STDERR_FILENO, "clCreateCommandQueue error: %d\n", err);
	return (err);
}