gxx = gcc
terget = user.o Ctool.o Cstr.o

vpath %.h ../header
vpath %.c ../src

all: user

user: $(terget)
	$(gxx) -o user $(terget)
	
%.o:%.c
	$(gxx) -c $^

