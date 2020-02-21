all: barber customer launcher

barber: barber.c
	gcc -o barber barber.c -std=c99 -Wall -lpthread -lm

customer: customer.c
	gcc -o customer customer.c -std=c99 -Wall -lpthread -lm

launcher: launcher.c
	gcc -o launcher launcher.c -std=c99 -Wall -lpthread -lm

clean:
	-rm barber customer launcher
