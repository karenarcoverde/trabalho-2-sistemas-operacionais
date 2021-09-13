# the compiler
CC = gcc

# compiler flags:
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall -Werror -D_POSIX_THREAD_SEMANTICS -g
LDFLAGS = -pthread

# the build target executable:
TARGET = trabalho2

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LDFLAGS)

clean:
	$(RM) $(TARGET)
