 
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
TESTOBJECTS := $(patsubst $(TESTDIR)/%,$(BUILDDIR)/%,$(TESTSOURCES:.$(SRCEXT)=.o))

CFLAGS := -g -Wall
LIB := -L bin
INC := -I $(INCDIR)
RPATH := -Wl,-rpath ./bin

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) -shared $^ -o $(TARGET) $(LIB)"; $(CC) -shared $^ -o $(TARGET) $(LIB)

$(OBJECTS):$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) $(INCDIR)/*
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) -fpic $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) -fpic $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR)/* $(BINDIR)/*"; $(RM) -r $(BUILDDIR)/* $(BINDIR)/*

$(TESTOBJECTS):$(BUILDDIR)/%.o: $(TESTDIR)/%.$(SRCEXT) $(INCDIR)/*
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Tests
bin/tester: $(TESTOBJECTS) $(TARGET)
	$(CC) $(CFLAGS) $(TESTOBJECTS) -l$(LIBNAME) $(INC) $(LIB) $(RPATH) -o bin/tester


.PHONY: clean