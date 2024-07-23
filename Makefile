CC := gcc
S := src/
DOCS_OUT_DIR = docs

FLAGS = -I. -std=c23
OBJ_FILES := str/str.o file/file.o
TEST_EXES := str/test.exe file/test.exe

libfiesta.a: $(OBJ_FILES)
	ar rcs -o $@ $^

%.o: %.c
	$(CC) -c $< -o $@ $(FLAGS)

%/test.exe: %/test.c
	$(CC) $< -o $@ -L. -lfiesta -I.. -std=c23

dbg: FLAGS += -g
dbg: libfiesta.a

opt: FLAGS += -O2
opt: libfiesta.a

dbgopt: FLAGS += -Og
dbgopt: $(B)strvm.exe

tests: libfiesta.a
tests: $(TEST_EXES)

.PHONY: docs

docs:
	python docs/make_docs.py $(DOCS_OUT_DIR)

lib: libfiesta.a
	$(RM) $(OBJ_FILES) $(TEST_EXES)

clean:
	$(RM) libfiesta.a $(OBJ_FILES) $(TEST_EXES)