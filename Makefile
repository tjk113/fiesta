CC := gcc
S := src/

FLAGS = -I. -std=c17

OBJ_FILES := str/str.o

libfiesta.a: $(OBJ_FILES)
	ar rcs -o $@ $^

%.o: %.c
	$(CC) -c $< -o $@ $(FLAGS)

dbg: FLAGS += -g
dbg: libfiesta.a

opt: FLAGS += -O2
opt: libfiesta.a

lib: libfiesta.a
	$(RM) $(OBJ_FILES)

clean:
	$(RM) libfiesta.a $(OBJ_FILES)