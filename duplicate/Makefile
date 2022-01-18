#SRC_EXT=c
CC= gcc -Wall
#HDR_EXT=h

SRC= duplicate.c
#SRC=$(shell ls | grep -E '*\.$(SRC_EXT)')
#HDR=$(shell ls | grep -E '*\.$(HDR_EXT)')
BIN=dup
$(BIN): $(SRC)
	$(CC) -Wextra -std=gnu99 $(SRC) -o $(BIN)






clean:
	rm -f $(BIN) *.o
	rm -rf Classic_copy.bmp
