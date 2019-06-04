EXEC=program

CC=g++
CFLAGS=-Wall -c
LFLAGS=-lcurl -lwiringPi


%.o: %.cpp
	$(CC) -o $@ $< $(CFLAGS)

%.o: %.c
	$(CC) -o $@ $< $(CFLAGS)

${EXEC}: main.o cimis.o temp_and_humidity.o
	$(CC) -o $@ $^ $(LFLAGS)

cimis_d: CFLAGS+=-g -DDEBUG
cimis_d: cimis.o
	$(CC) -o $@ $^ $(LFLAGS)

temp_and_humidity_d: CFLAGS+=-g -DDEBUG
temp_and_humidity_d: temp_and_humidity.o
	$(CC) -o $@ $^ $(LFLAGS)
clean:
	rm -f ./${EXEC}
	rm -f ./cimis_d
	rm -f ./temp_and_humidity
	rm -f ./*.o
