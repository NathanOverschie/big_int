 
CC := g++ # This is the main compiler
SRCDIR := src
BUILDDIR := build
BINDIR := bin
INCDIR := include
TESTDIR := test
LIBNAME := bigint
TARGET := $(BINDIR)/lib$(LIBNAME).so

 
SRCEXT := cpp
INCEXT := h
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

TESTSOURCES := $(shell find $(TESTDIR) -type f -name *.$(SRCEXT))
TESTOBJECTS := $(patsubst $(TESTDIR)/%,$(BUILDDIR)/%,$(TESTSOURCES:.$(SRCEXT)=.testo))
TESTS := $(patsubst $(TESTDIR)/%,$(BINDIR)/%,$(TESTSOURCES:.$(SRCEXT)=.test))

CFLAGS := -g -Wall
LIB := -L bin
INC := -I $(INCDIR)
RPATH := -Wl,-rpath ./bin

$(TARGET): $(OBJECTS)
	@echo "\n\t\tLinking...\n\n"
	$(CC) -shared $^ -o $(TARGET) $(LIB)

$(OBJECTS):$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) $(INCDIR)/*
	@echo "\n\t\tCompiling $*\n\n"
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -fpic $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	$(RM) -r $(BUILDDIR)/* $(BINDIR)/*

$(TESTOBJECTS):$(BUILDDIR)/%.testo: $(TESTDIR)/%.$(SRCEXT) $(INCDIR)/*
	@echo "\n\t\tCompiling test $*\n\n"
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Tests
$(TESTS):$(BINDIR)/%.test: $(BUILDDIR)/%.testo $(TARGET)
	@echo "\n\t\tLinking tests...\n\n"
	$(CC) $(CFLAGS) $< -l$(LIBNAME) $(INC) $(LIB) $(RPATH) -o $@


test: $(TESTS)

.PHONY: clean