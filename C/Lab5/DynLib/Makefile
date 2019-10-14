lab5-0 : lab5-0.o liblab5.so
	gcc -o lab5-0 lab5-0.o -ldl -Wall -std=c99
	LD_LIBRARY_PATH=/home/john/Eltex/C/Lab5/DynLib
	export LD_LIBRARY_PATH
lab5-0.o: lab5-0.c
	gcc -c lab5-0.c -Wall -std=c99
liblab5.so : lab5-1.o lab5-2.o
	gcc -shared -o liblab5.so lab5-1.o lab5-2.o -Wall -std=c99
lab5-1.o lab5-2.o : lab5-1.c lab5-2.c
	gcc -fPIC -c lab5-1.c lab5-2.c -Wall -std=c99
clean :
	rm lab5-1.o lab5-2.o liblab5.so lab5-0 lab5-0.o
