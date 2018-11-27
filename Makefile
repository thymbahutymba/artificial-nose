CC = gcc
LDFLAGS = -pthread -lrt
CFLAGS = -Wall -Wextra
ALLEGRO =  `allegro-config --libs`
OBJECTS = graphics.o
DOCKERFLAGS = -it --rm --privileged --cap-add=sys_nice -e DISPLAY="10.10.10.10:0.0"

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o graphics $^ $(ALLEGRO)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm *.o

docker: build run

build:
	docker build --rm -f "Dockerfile" -t artificial-nose:latest .

run:
	docker run $(DOCKERFLAGS) artificial-nose