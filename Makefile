all: 

my_service: _service_ip.c
	gcc -o service _service_ip.c -lpthread
tooldebug:
	gcc -o tool debugbatctl.c
toolreadfile:
	gcc -o fir  fir.c
clean:
	rm -rf server client if myif thread condition cancel service *~
