OPENCL = -I/usr/local/cuda/include -framework OpenCL

all:
	g++ test.cpp $(OPENCL) 
