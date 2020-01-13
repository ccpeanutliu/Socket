all:
	g++ server.cpp -L /usr/bin/openssl -l ssl -l crypto -o server.o -lpthread
	g++ newClient.cpp -L /usr/bin/openssl -l ssl -l  crypto -o newClient.o
	openssl genrsa -out pri.pem 1024
	openssl rsa -in pri.pem -pubout -out pub.pem
	openssl genrsa -out pri1.pem 1024
	openssl rsa -in pri1.pem -pubout -out pub1.pem
test:
	openssl genrsa -out pri.pem 1024
	openssl rsa -in pri.pem -pubout -out pub.pem
	g++ rsa.cpp -L /usr/bin/openssl -l ssl -l  crypto -o rsa.o
no_encrypt:
	g++ beforeRSA.cpp -o NOE.o
s:
	g++ server.cpp -o server.o
c:
	g++ newClient.cpp -o newClient.o
clear:
	rm *.o
	rm *.pem