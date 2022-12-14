TARGET = server

# compiling flags
CC     = gcc
CFLAGS = -std=c17 -Wall -I.7
LFLAGS = -Wall -I. -lm

# sources + build directory
SRCDIR = src
OBJDIR = obj
BINDIR = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CC) $(LFLAGS) -o $@ $(OBJECTS)

all: prebuild $(BINDIR)/$(TARGET)

prebuild:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(BINDIR)

test: all
	@pip3 install -r test/integration/requirements.txt
	@python3 -m pytest test/integration/

.PHONY: all test clean
clean:
	rm -rf $(BINDIR)/* $(OBJDIR)/* 
