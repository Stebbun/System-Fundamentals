CC := gcc
SRCD := src
TSTD := tests
BLDD := build
BIND := bin
INCD := include

ALL_SRCF := $(shell find $(SRCD) -type f -name *.c)
ALL_OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(ALL_SRCF:.c=.o))
MAIN_FILES := build/server.o
FUNC_FILES := $(filter-out $(MAIN_FILES), $(ALL_OBJF))
TEST_SRC := $(shell find $(TSTD) -type f -name *.c)

INC := -I $(INCD)

SERVER_EXEC := server
SERVER_SRC := $(SERVER_EXEC).c
TEST_EXEC := tests

CFLAGS := -Wall -Werror -Wno-unused-function -Wno-empty-translation-unit
DFLAGS := -g -DDEBUG -DCOLOR
STD := -std=gnu11
TEST_LIB := -lcriterion
LIBS := -pthread

CFLAGS += $(STD)

.PHONY: clean all

all: setup $(SERVER_EXEC) $(TEST_EXEC)

debug: CFLAGS += $(DFLAGS)
debug: all

test: setup $(TEST_EXEC)

setup:
	mkdir -p bin build

$(SERVER_EXEC): $(ALL_OBJF)
	$(CC) $(CFLAGS) $(INC) $(FUNC_FILES) $(SRCD)/$(SERVER_EXEC).c -o $(BIND)/$@ $(LIBS)

$(TEST_EXEC): $(FUNC_FILES)
	$(CC) $(CFLAGS) $(INC) $(FUNC_FILES) $(TEST_SRC) -o $(BIND)/$@ $(TEST_LIB) $(LIBS)

$(BLDD)/%.o: $(SRCD)/%.c
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) -r $(BLDD) $(BIND)
