
#CC:=ccache $(CC)
CFLAGS+= -Wall -Werror -std=gnu99 -g
LDFLAGS=-lpthread

#HW=prgsem
BINARIES=comp

CFLAGS+=$(shell sdl2-config --cflags)
LDFLAGS+=$(shell sdl2-config --libs) 


all: $(BINARIES)

OBJS=$(patsubst %.c,%.o,$(wildcard *.c))

comp: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(BINARIES) $(OBJS)
