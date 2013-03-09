objects = main.o
output = updump
CFLAGS ?= -Wall -O2 -g -std=c89

ifeq ($(shell uname -m),i386)
	objects += x86cpuid.o
	CFLAGS += -DUP_I386
endif
ifeq ($(shell uname -m),x86_64)
	objects += x86cpuid.o
	CFLAGS += -DUP_X86_64
endif

$(output) : $(objects)
	$(CC) $(LDFLAGS) $(objects) -o $(output)

$(objects) : %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	rm -f $(objects) $(output)

.PNONY : clean
