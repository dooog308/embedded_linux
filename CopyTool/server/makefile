gxx = arm-linux-gnueabihf-gcc
terget = server.o Ctool.o Cstr.o

vpath %.h ../header
vpath %.c ../src

all: server.arm

server.arm: $(terget)
	$(gxx) -o server.arm $(terget) -lpthread
	
%.o:%.c
	$(gxx) -c $^

