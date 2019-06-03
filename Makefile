EXEC=program

CC=g++
CFLAGS=-Wall -c
LFLAGS=-lcurl


%.o: %.cpp
	$(CC) -o $@ $< $(CFLAGS)

cimis_d:LFLAGS+=-Wall -g -DDEBUG
cimis_d: cimis.cpp
	$(CC) -o $@ $^ $(LFLAGS)

${EXEC}: main.o cimis.o
	$(CC) -o ${EXEC} $^ $(LFLAGS)

clean:
	rm -f ./${EXEC}
	rm -f ./cimis_d
	rm -f ./*.o
