CC := gcc
SRC_DIR := src
INC_DIR := include/fiesta
DOCS_DIR := docs
LIB_DIR := lib
TESTS_DIR := tests
PYTHON_EXE := python

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

override FLAGS += -I$(INC_DIR) -std=c23
OBJ_FILES := $(LIB_DIR)/file.o $(LIB_DIR)/str.o
TEST_EXES := $(patsubst $(TESTS_DIR)/file/%.c, $(TESTS_DIR)/file/%$(EXE_EXT), $(wildcard $(TESTS_DIR)/file/*.c)) \
			 $(patsubst $(TESTS_DIR)/str/%.c, $(TESTS_DIR)/str/%$(EXE_EXT), $(wildcard $(TESTS_DIR)/str/*.c)) \
			 $(patsubst $(TESTS_DIR)/optional/%.c, $(TESTS_DIR)/optional/%$(EXE_EXT), $(wildcard $(TESTS_DIR)/optional/*.c))

$(LIB_DIR)/libfiesta.a: $(OBJ_FILES)
	ar rcs -o $@ $^

$(LIB_DIR)/%.o: $(SRC_DIR)/%.c | make_lib_dir
	$(CC) -c $< -o $@ $(FLAGS)

$(TESTS_DIR)/file/%$(EXE_EXT): $(TESTS_DIR)/file/%.c | make_tests_dir
	$(CC) $< -o $@ -L$(LIB_DIR) -lfiesta -Itests $(FLAGS)

$(TESTS_DIR)/str/%$(EXE_EXT): $(TESTS_DIR)/str/%.c | make_tests_dir
	$(CC) $< -o $@ -L$(LIB_DIR) -lfiesta -Itests $(FLAGS)

$(TESTS_DIR)/optional/%$(EXE_EXT): $(TESTS_DIR)/optional/%.c | make_tests_dir
	$(CC) $< -o $@ -L$(LIB_DIR) -lfiesta -Itests $(FLAGS)

make_lib_dir:
	$(MKDIR) $(LIB_DIR)

make_tests_dir:
	$(MKDIR) $(TESTS_DIR)

make_docs_dir:
	$(MKDIR) $(DOCS_DIR)

all: lib docs test

dbg: FLAGS += -g
dbg: lib

opt: FLAGS += -O2
opt: lib

dbgopt: FLAGS += -Og
dbgopt: lib

lib: $(LIB_DIR)/libfiesta.a

test: lib $(TEST_EXES)
	@$(PYTHON_EXE) tools/test.py

.PHONY: docs

docs: | make_docs_dir
	$(PYTHON_EXE) tools/make_docs.py $(DOCS_DIR)

clean:
	$(RM) $(LIB_DIR)/libfiesta.a $(OBJ_FILES) $(TEST_EXES) $(DOCS_DIR)/index.html