#include <OCL.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

cl_int
openCLBuildProgram(cl_program program, cl_device_id device_id)
{
	cl_int					err;
	cl_program_build_info	status;
	size_t					log_size;
	char					*log;

	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &status, NULL);
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		if (!(log = (char *)malloc(sizeof(char) * (log_size + 1))))
			return (err);
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size + 1, log, NULL);
		dprintf(STDERR_FILENO, "Build failed; error=%d, status=%d, programLog:\n%s",  err, status, log);
		free(log);
	}
	return (err);
}