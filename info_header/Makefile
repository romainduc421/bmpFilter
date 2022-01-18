#SRC_EXT=c
CC= gcc -Wall
#HDR_EXT=h

SRC= info_header.c
#SRC=$(shell ls | grep -E '*\.$(SRC_EXT)')
#HDR=$(shell ls | grep -E '*\.$(HDR_EXT)')
HDR=bitmap.h
BIN=info

$(BIN): $(SRC) $(HDR)
	$(CC) -Wextra -std=gnu99 $(SRC) -o $(BIN)








clean:
	rm -f $(BIN) *.o
