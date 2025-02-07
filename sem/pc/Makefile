
#CC:=ccache $(CC)
CFLAGS+= -Wall -Werror -std=gnu99 -g
LDFLAGS=-lpthread

#HW=prgsem
BINARIES=prgsem

CFLAGS+=$(shell sdl2-config --cflags)
LDFLAGS+=$(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf -lm


all: $(BINARIES)

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

prgsem: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(BINARIES) $(OBJS)
