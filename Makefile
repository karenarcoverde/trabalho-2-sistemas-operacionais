CC = g++

CFLAGS  = -Wall -std=c++11 -pthread

TARGET = trabalho2


all: $(TARGET)

$(TARGET): $(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).cpp
 	
clean:
	$(RM) $(TARGET)

# # Esse Makefile foi feito com modificacoes para o caso do c++ e o uso do pthread a partir do drive que contem o codigo fornecido pelo professor.
# https://drive.google.com/file/d/1wWEsfPYXpFlcPJZywxCdjW_JX4oEA-aJ/view?usp=sharing 