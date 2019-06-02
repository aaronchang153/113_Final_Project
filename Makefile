EXEC=program

CC=g++
CFLAGS=-Wall -c
LFLAGS=-Wall -lcurl


%.o: %.cpp
	$(CC) $< $(CFLAGS)

${EXEC}: main.o cimis.o
	$(CC) -o ${EXEC} $^ $(LFLAGS)

clean:
	rm ./${EXEC}
	rm ./*.o
