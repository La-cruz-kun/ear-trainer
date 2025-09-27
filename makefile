CFLAGS = -Wall -Wextra -ggdb -pedantic 
LIBS = -lraylib -lGL -lm -lpthread
SRC_DIR = include
INC_SRC = $(wildcard  $(SRC_DIR)/*.c)
CC = clang

ear_trainer: src/main.c
	$(CC) -fPIC $(CFLAGS) -Llib -Iinclude -shared $(INC_SRC) src/plug.c -o build/libplug.so $(LIBS)
	$(CC) $(CFLAGS) -Llib -Iinclude $(INC_SRC) src/main.c -o build/ear_trainer $(LIBS)


test: src/test.c
	$(CC) $(CFLAGS) -Llib  src/test.c -o build/test $(LIBS)

test2: src/test2.c
	$(CC) $(CFLAGS) -Llib  src/test2.c -o build/test2 $(LIBS)
