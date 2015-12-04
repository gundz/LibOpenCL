OS := $(shell uname)

ifeq ($(OS), Linux)
OPENCL = -I $(AMDAPPSDKROOT)/include/ -L $(AMDAPPSDKROOT)/lib/x86_64/sdk/ -lOpenCL
else
	OPENCL = -I/usr/local/cuda/include -framework OpenCL
endif

all:
	g++ test.cpp $(OPENCL) 
