lab5-0 : lab5-0.c liblab5.a
	gcc -o lab5-0 lab5-0.c -L. -llab5 -Wall -std=c99
liblab5.a : lab5-1.o lab5-2.o
	ar rc liblab5.a lab5-1.o lab5-2.o
	ranlib liblab5.a
lab5-2.o : lab5-2.c
	gcc -Wall -c lab5-2.c
lab5-1.o : lab5-1.c
	gcc -Wall -c lab5-1.c
clean :
	rm liblab5.a lab5-1.o lab5-2.o lab5-0
