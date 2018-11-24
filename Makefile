CC = gcc
LDFLAGS = -lpthread
CFLAGS = -Wall -Wextra `allegro-config --libs`

.PHONY: all
all:
	$(CC) $(LDFLAGS) -o graphics graphics.c $(CFLAGS) 

.PHONY: clean
clean:
	rm *.o

.PHONY: docker
docker:
	docker build --rm -f "Dockerfile" -t artificial-nose:latest .
	docker run -ti --rm -e DISPLAY="192.168.1.197:0.0" artificial-nose ./graphics
