#
# command-line: make STATE_COUNT=<#>
# 

OSNAME := $(shell uname)

ifeq ($(OSNAME),Linux)
	EXT				:= so
	CUDA_SDK_PATH	:= /opt/NVIDIA_CUDA_SDK
	JAVA_HOME 		:= /usr/java/default
	INCLUDES		+= -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -Iinclude
endif

ifeq ($(OSNAME),Darwin)
	EXT 			:=	jnilib
	CUDA_SDK_PATH 	:= /Developer/CUDA
	JAVA_HOME		:= /System/Library/Frameworks/JavaVM.framework
	INCLUDES		+= -I$(JAVA_HOME)/Headers -Iinclude
endif

CUDA_INSTALL_PATH	:= /usr/local/cuda

BASEDIR=.
TARGET_DIR=$(BASEDIR)/lib
HEADER=$(BASEDIR)

STATE_COUNT = 60
SCALE = 1E+0f
		       
EXECUTABLE_D	:= $(TARGET_DIR)/libBEAGLE-$(STATE_COUNT)-D.$(EXT)
EXECUTABLE_S	:= $(TARGET_DIR)/libBEAGLE-$(STATE_COUNT)-S.$(EXT)

CUFILES	:= ./src/CUDA/CUDASharedFunctions.c	\
		   ./src/CUDA/CUDASharedFunctions_kernel.cu \
		   ./src/CUDA/beagleCUDA.c \
		   ./src/CUDA/TransitionProbabilities_kernel.cu \
		   ./src/CUDA/Peeling_kernel.cu \
		   ./java/JNI/beagle_BeagleJNIWrapper.c
		         		       		      
############################################################

# Machine-dependent includes
INCLUDES  += -I. -I$(CUDA_INSTALL_PATH)/include -I$(CUDA_SDK_PATH)/common/inc -I$(COMMONDIR)/inc \
	  	     -I$(HEADER) -I$(TARGET_DIR)classes 
# Libs
LIB       := -L$(CUDA_INSTALL_PATH)/common/lib
DOLINK    := -lcuda -lcudart

all : device

directories :
	if test ! -e $(TARGET_DIR); then mkdir $(TARGET_DIR); fi

clean:
	rm -f $(CCOFILES) $(EXECUTABLE)
	
device: directories
	@echo "using device mode!"
	nvcc -O3 -shared -DSTATE_COUNT=$(STATE_COUNT) -DSCALE=$(SCALE)\
		 --ptxas-options=-v \
		 --compiler-options -std=c99 \
		 --compiler-options -fPIC \
		 --compiler-options -funroll-loops \
		 -o $(EXECUTABLE_S)  $(CUFILES) $(INCLUDES) $(LIB) $(DOLINK)
		 
device-double: directories
	@echo "using device mode!"
	nvcc -O3 -shared -DSTATE_COUNT=$(STATE_COUNT) -DSCALE=$(SCALE)\
		 --ptxas-options=-v \
		 --compiler-options -fPIC \
		 --compiler-options -funroll-loops \
		 -arch sm_13 -DDOUBLE_PRECISION \
		 -o $(EXECUTABLE_D)  $(CUFILES) $(INCLUDES) $(LIB) $(DOLINK)

debug: directories
	@echo "using debug mode!"
	nvcc -DDEBUG -O3 -shared -DSTATE_COUNT=$(STATE_COUNT) -DSCALE=$(SCALE)\
	 	 --compiler-options -std=c99 \
		 -o $(EXECUTABLE_S) $(CUFILES) $(INCLUDES) $(LIB) $(DOLINK)
		 
debug-double: directories
	@echo "using debug mode!"
	nvcc -DDEBUG -O3 -shared -DSTATE_COUNT=$(STATE_COUNT) -DSCALE=$(SCALE)\
		 -arch sm_13 -DDOUBLE_PRECISION \
		 -o $(EXECUTABLE_D) $(CUFILES) $(INCLUDES) $(lib) $(DOLINK)		 

emulation: directories
	@echo "using emulation mode!"
	nvcc -g -DDEBUG -DKERNEL_PRINT_ENABLED -O3 -shared -DSTATE_COUNT=$(STATE_COUNT) -DSCALE=$(SCALE)\
	     --compiler-options -std=c99 \
		 -o $(EXECUTABLE_S) $(CUFILES) $(INCLUDES) $(lib) $(DOLINK) -deviceemu
		 
emulation-double: directories
	@echo "using emulation mode!"
	nvcc -g -DDEBUG -DKERNEL_PRINT_ENABLED -O3 -shared -DSTATE_COUNT=$(STATE_COUNT) -DSCALE=$(SCALE)\
	     -arch sm_13 -DDOUBLE_PRECISION \
		 -o $(EXECUTABLE_D) $(CUFILES) $(INCLUDES) $(lib) $(DOLINK) -deviceemu		 
	
	



