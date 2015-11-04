
# find the os
UNAME_S := $(shell uname -s)

# main compiler
CXX := g++
# CXX := clang --analyze # and comment out the linker last line for sanity

# define the directories
SRCDIR := src
INCDIR := include
BUILDDIR := build
BINDIR := bin
LIBDIR := lib

# define common variables
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -maxdepth 1 -type f \( -iname "*.$(SRCEXT)" ! -iname "*main-*.$(SRCEXT)" \) )
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CXXFLAGS := -g # -Wall

INC := $(shell find $(INCDIR) -maxdepth 1 -type d -exec echo -I {}  \;)
LIB := -lonioni2c

ifeq ($(UNAME_S),Darwin)
	# only add this when compiling on OS X
	LIB := -L$(LIBDIR) $(LIB)
	INC += -I $(LIBDIR)
endif
ifeq ($(CXX),mips-openwrt-linux-uclibc-gcc)
	CUSTFLAGS := -fPIC
endif

# define specific binaries to create
APP0 := ads1x15-driver
TARGET := $(BINDIR)/$(APP0)

# libraries
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	@echo " Linking..."
	@echo " $(CXX) $^ -o $(TARGET) $(LIB)"; $(CXX) $^ -o $(TARGET) $(LIB)

# generic: build any object file required
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	@echo "building $@"
	@echo " $(CXX) $(CXXFLAGS) $(CUSTFLAGS) $(INC) -c -o $@ $<"; $(CXX) $(CXXFLAGS) $(CUSTFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(BINDIR)"; $(RM) -r $(BUILDDIR) $(BINDIR)

bla:
	@echo "LIB: $(LIB)"
	@echo "INC: $(INC)"


# Tests
tester:
	$(CXX) $(CXXFLAGS) test/tester.cpp $(INC) $(LIB) -o bin/tester

# Spikes
#ticket:
#  $(CXX) $(CXXFLAGS) spikes/ticket.cpp $(INC) $(LIB) -o bin/ticket

.PHONY: clean
