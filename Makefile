all: pong

pong: main.cpp
	g++ -O2 main.cpp -lraylib -o pong

run: pong
	./pong

clean:
	rm pong

.PHONY: all pong run clean
