LIBS:= $(wildcard lib/*.c)
OBJS:= $(patsubst %.c, %.o, $(subst lib/,,$(wildcard lib/*.c)))
EXEC:= $(patsubst %.c, %.exe, $(subst src/,,$(wildcard src/*.c)))
TEST:= $(patsubst %.c, %.out, $(subst test/,,$(wildcard test/*.c)))

CC:=gcc
CFLAGS:=-fPIC -O2 -std=gnu99 -g3
INC:=-I include/

all: $(OBJS) $(EXEC) 
test: $(OBJS) $(TEST)

%.o: lib/%.c 
	$(CC) $(CFLAGS) $(DFLAGS) -c $^ $(INC) -lpthread 

%.exe: src/%.c $(OBJS)
	$(CC) $(CFLAGS) $(INC) -o $@ $(OBJS) $< $(INC) -lpthread

%.out: test/%.c $(OBJS)
	$(CC) $(CFLAGS) $(INC) -o $@ $(OBJS) $< $(INC) -lpthread

.PHONY=clean

clean:
	rm -rf $(OBJS) $(EXEC) $(TEST)