CC := gcc
S := src/
DOCS_OUT_DIR = docs

FLAGS = -I. -std=c17
OBJ_FILES := str/str.o
TEST_FILES := str/test.exe

libfiesta.a: $(OBJ_FILES)
	ar rcs -o $@ $^

%.o: %.c
	$(CC) -c $< -o $@ $(FLAGS)

%/test.exe: %/test.c
	$(CC) $< -o $@ -L. -lfiesta -I.. -std=c17

%/%.h: 

dbg: FLAGS += -g
dbg: libfiesta.a

opt: FLAGS += -O2
opt: libfiesta.a

dbgopt: FLAGS += -Og
dbgopt: $(B)strvm.exe

tests: libfiesta.a
tests: str/test.exe

docs: %/%.h $(DOCS_OUT_DIR)/index.html
$(DOCS_OUT_DIR)/index.html:
	python docs/make_docs.py $(DOCS_OUT_DIR)

lib: libfiesta.a
	$(RM) $(OBJ_FILES) $(TEST_FILES)

clean:
	$(RM) libfiesta.a $(OBJ_FILES) $(TEST_FILES)