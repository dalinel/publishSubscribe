
all: mediator subscriber1 subscriber2 subscriber3 publisher1 publisher2 main

mediator: circular_buffer.o mediator.o
	gcc -o mediator circular_buffer.o mediator.o

mediator.o: mediator.c
	gcc -o mediator.o -c mediator.c -Wall

circBuff2.o: circular_buffer.c
	gcc -o circular_buffer.o -c circular_buffer.c -Wall

subscriber1: subscriber1.c
	gcc -Wall subscriber1.c -o subscriber1

subscriber2: subscriber2.c
	gcc -Wall subscriber2.c -o subscriber2

subscriber3: subscriber3.c
	gcc -Wall subscriber3.c -o subscriber3

publisher1: publisher1.c
	gcc -Wall publisher1.c -o publisher1

publisher2: publisher2.c
	gcc -Wall publisher2.c -o publisher2

main: main.c
	gcc -Wall main.c -o main

clean:
	rm -rf *.o

mrproper: clean
	rm -rf mediator subscriber1 subscriber2 subscriber3 publisher1 publisher2 main
