#SRC_EXT=c
CC= gcc -Wall
#HDR_EXT=h
SRC2= fils.c
SRC= pipe_fileBMP.c
#SRC=$(shell ls | grep -E '*\.$(SRC_EXT)')
#HDR=$(shell ls | grep -E '*\.$(HDR_EXT)')
HDR=bitmap.h
BIN=prog
BIN2=fils
$(BIN): $(SRC) $(HDR) $(BIN2)
	$(CC) -Wextra -std=gnu99 $(SRC) -o $(BIN)
$(BIN2): $(SRC2) $(HDR)
	$(CC) -Wextra -std=gnu99 $(SRC2) -o $(BIN2)







clean:
	rm -f $(BIN) $(BIN2) *.o
	rm -rf copieClassic_final*
