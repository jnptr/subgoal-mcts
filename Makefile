CXX := g++
#CXX := icpc
#CXX := clang++

SRCDIR := src
BUILDDIR := build
DEPDIR := .d
TARGET_DIR := bin
MAIN_SUFFIX := _main

SRCEXT := cpp
HEADEREXT := hpp
MAIN_SOURCES := $(shell find $(SRCDIR) -type f -name *$(MAIN_SUFFIX).$(SRCEXT))
TARGETS := $(patsubst $(SRCDIR)/%,$(TARGET_DIR)/%,$(MAIN_SOURCES:$(MAIN_SUFFIX).$(SRCEXT)=))
OTHER_SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT) -not -name *$(MAIN_SUFFIX).$(SRCEXT))
OTHER_OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(OTHER_SOURCES:.$(SRCEXT)=.o))
ALL_SOURCES_AND_HEADERS := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT) -or -name *.$(HEADEREXT))
ALL_SOURCES := $(MAIN_SOURCES) $(OTHER_SOURCES)
DEPS := $(patsubst $(SRCDIR)/%,$(DEPDIR)/%,$(ALL_SOURCES:.$(SRCEXT)=.d))
INC := -I include
OS := $(shell uname)
LIB := -L/usr/local/lib -lboost_program_options -lboost_unit_test_framework

CXXFLAGS += -std=gnu++14 -Wall -fopenmp
CXX_DEBUG_FLAGS := -Wextra -D DEBUG -g
CXX_OPT_FLAGS := -O3 -march=native -D NDEBUG
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

ifeq "$(CXX)" "icpc"
  CXX_OPT_FLAGS += -ipo
else
  CXX_OPT_FLAGS += -flto
endif

opt: CXXFLAGS += $(CXX_OPT_FLAGS)
opt: $(TARGETS)

debug: CXXFLAGS += $(CXX_DEBUG_FLAGS)
debug: CXXFLAGS += -fdump-class-hierarchy
debug: $(TARGETS)

test: CXXFLAGS += $(CXX_DEBUG_FLAGS)
test: $(TARGETS)
test:
	bin/test --log_level=all --color_output

$(TARGET_DIR)/%: $(OTHER_OBJECTS) $(BUILDDIR)/%$(MAIN_SUFFIX).o
	@echo " Linking..."
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIB)
	$(if $(SUFFIX),@mv -f $@ $@$(SUFFIX),)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) $(DEPDIR)/%.d
	@mkdir -p $(@D)
	@mkdir -p $(DEPDIR)/$(*D)
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(INC) -c -o $@ $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;

clean:
	@echo " Cleaning...";
	$(shell rm -r $(BUILDDIR) $(TARGETS) $(DEPDIR))
  $(shell	find . -name .DS_Store | xargs rm -f)

loc:
	@echo " Lines of code:"
	find . "(" -name "*.h" -or -name "*.hpp" -or -name "*.cpp" ")" -print0 | xargs -0 wc -l

tidy:
	clang-tidy $(ALL_SOURCES_AND_HEADERS) -fix -- -std=c++14
	clang-format -i  $(ALL_SOURCES_AND_HEADERS)

format:
	clang-format -i  $(ALL_SOURCES_AND_HEADERS)

.PHONY: opt
.PHONY: debug
.PHONY: test
.PHONY: clean
.PHONY: loc
.PHONY: tidy
.PHONY: format
.PRECIOUS: $(DEPDIR)/%.d
.PRECIOUS: $(BUILDDIR)/%.o

-include $(DEPS)
