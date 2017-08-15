CC := gcc
SRCD := src
TSTD := tests
BLDD := build
BIND := bin
INCD := include

SF_UTIL := build/sfutil.o

ALL_SRCF := $(shell find $(SRCD) -type f -name *.c)
ALL_OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(ALL_SRCF:.c=.o))
ALL_OBJF += $(SF_UTIL)
FUNC_FILES := $(filter-out build/main.o, $(ALL_OBJF))
ALL_OBJF_XCPT_SFUTIL := $(filter-out $(SF_UTIL), $(ALL_OBJF))
TEST_SRC := $(shell find $(TSTD) -type f -name *.c)

INC := -I $(INCD)

EXEC := main
TEST_EXEC := test_$(EXEC)

CFLAGS := -Wall -Werror
DFLAGS := -g -DDEBUG -DCOLOR
STD := -std=gnu11
TEST_LIB := -lcriterion
LIBS := -lreadline -pthread -ljansson

CFLAGS += $(STD)

.PHONY: clean all

debug: CFLAGS += $(DFLAGS)
debug: all

test: setup $(TEST_EXEC)

all: setup $(EXEC) $(TEST_EXEC)

setup:
	mkdir -p bin build

$(EXEC): $(ALL_OBJF)
	$(CC) $^ -o $(BIND)/$@

$(TEST_EXEC): $(FUNC_FILES)
	$(CC) $(CFLAGS) $(INC) $(FUNC_FILES) $(TEST_SRC) $(TEST_LIB) -o $(BIND)/$@

$(BLDD)/%.o: $(SRCD)/%.c
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) -r $(ALL_OBJF_XCPT_SFUTIL) $(BIND)
