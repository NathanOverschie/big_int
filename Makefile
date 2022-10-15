 
CC := g++ # This is the main compiler
SRCDIR := src
BUILDDIR := build
TESTDIR := test
LIBNAME := bigint
TARGET := bin/lib$(LIBNAME).so

 
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

TESTSOURCES := $(shell find $(TESTDIR) -type f -name *.$(SRCEXT))
TESTOBJECTS := $(patsubst $(TESTDIR)/%,$(BUILDDIR)/%,$(TESTSOURCES:.$(SRCEXT)=.o))

CFLAGS := -g -Wall
LIB := -L bin
INC := -I include
RPATH := -Wl,-rpath ./bin

$(TARGET).so: $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) -shared $^ -o $(TARGET) $(LIB)"; $(CC) -shared $^ -o $(TARGET) $(LIB)

$(OBJECTS):$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) -fpic $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) -fpic $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

$(TESTOBJECTS):$(BUILDDIR)/%.o: $(TESTDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

# Tests
tester: $(TESTOBJECTS) $(TARGET)
	$(CC) $(CFLAGS) $(TESTOBJECTS) -l$(LIBNAME) $(INC) $(LIB) $(RPATH) -o bin/tester


.PHONY: clean