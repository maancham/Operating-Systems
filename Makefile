all: GoodreadsSerial.o

GoodreadsSerial.o: serial.cpp
	g++ -std=c++11 -w serial.cpp -o GoodreadsSerial.out

clean:
	rm -f *.o GoodreadsSerial.out 