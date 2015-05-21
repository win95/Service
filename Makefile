all: 

my_service: _service_ip.c
	gcc -o service _service_ip.c -lpthread
clean:
	rm -rf server client if myif thread condition cancel service *~
