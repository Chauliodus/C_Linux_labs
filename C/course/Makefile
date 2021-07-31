CFLAGS  = -Wall -Os -ggdb -std=c99  

all : course_serv course_cli_1 course

amessage.pb-c.c : amessage.proto
	protoc-c --c_out=. amessage.proto

course_serv : course_serv.c functions.c amessage.pb-c.c
	gcc -o course_serv course_serv.c functions.c amessage.pb-c.c -lpthread -lprotobuf-c
	
course_cli_1 : course_cli_1.c functions.c amessage.pb-c.c
	gcc -o course_cli_1 course_cli_1.c functions.c amessage.pb-c.c -lpthread -lprotobuf-c
	
course : course.c
	gcc -o course course.c
	
clean :
	rm course course_serv course_cli_1 amessage.pb-c.c amessage.pb-c.h

