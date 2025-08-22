CC := gcc
SRC_DIR := src
DOCS_DIR := docs
LIB_DIR := lib
TESTS_DIR := tests

# Windows' mkdir doesn't have an
# equivalent to -p on Linux, so
# we have a batch script that
# implements this functionality.
ifeq ($(OS),Windows_NT)
    MKDIR := @tools/windows_mkdir.bat
	EXE_EXT := .exe
else
    MKDIR := @mkdir -p
	EXE_EXT := 
endif

FLAGS := -Iinclude/fiesta -std=c23 -Wunused-function
OBJ_FILES := $(LIB_DIR)/str.o $(LIB_DIR)/file.o
TEST_EXES := $(TESTS_DIR)/str/test$(EXE_EXT) $(TESTS_DIR)/file/test$(EXE_EXT)

$(LIB_DIR)/libfiesta.a: $(OBJ_FILES)
	ar rcs -o $@ $^

$(LIB_DIR)/%.o: $(SRC_DIR)/%.c | make_lib_dir
	$(CC) -c $< -o $@ $(FLAGS)

$(TESTS_DIR)/%/test$(EXE_EXT): $(TESTS_DIR)/%/test.c | make_tests_dir
	$(CC) $< -o $@ -L$(LIB_DIR) -lfiesta -Iinclude/fiesta -std=c23

make_lib_dir:
	$(MKDIR) $(LIB_DIR)

make_tests_dir:
	$(MKDIR) $(TESTS_DIR)

make_docs_dir:
	$(MKDIR) $(DOCS_DIR)

all: lib tests docs

dbg: FLAGS += -g
dbg: lib

opt: FLAGS += -O2
opt: lib

dbgopt: FLAGS += -Og
dbgopt: lib

lib: $(LIB_DIR)/libfiesta.a

tests: lib $(TEST_EXES)

.PHONY: docs

docs: | make_docs_dir
	python tools/make_docs.py $(DOCS_DIR)

clean:
	$(RM) $(LIB_DIR)/libfiesta.a $(OBJ_FILES) $(TEST_EXES) $(DOCS_DIR)/index.html