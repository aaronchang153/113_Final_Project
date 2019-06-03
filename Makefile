EXEC=program

CC=g++
CFLAGS=-Wall -c
LFLAGS=-lcurl


%.o: %.cpp
	$(CC) -o $@ $< $(CFLAGS)

${EXEC}: main.o cimis.o
	$(CC) -o ${EXEC} $^ $(LFLAGS)

cimis_d: CFLAGS+=-g -DDEBUG
cimis_d: cimis.o
	$(CC) -o $@ $^ $(LFLAGS)

clean:
	rm -f ./${EXEC}
	rm -f ./cimis_d
	rm -f ./*.o
