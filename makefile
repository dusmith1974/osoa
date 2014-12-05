CXXFLAGS = -DBOOST_ALL_DYN_LINK
CXXFLAGS += -Wall -Wextra -ansi -pedantic -Weffc++ -Wshadow -Werror
CXXFLAGS += -Wno-error=effc++ # for boost and other libs.
CXXFLAGS += -std=c++11

ITEM = item
SERVICE = service
COMMS = $(SERVICE)/comms
UTIL = util
TEST = test
BASE = .

PRJS := $(ITEM) $(SERVICE) $(COMMS) $(UTIL) $(BASE) $(TEST)

LIB_OSOA_BASE = libosoa-mt
ifeq ($(MAKECMDGOALS), debug)
LIB_OSOA_SUFFIX = -d.a
CONFIG=debug
else
LIB_OSOA_SUFFIX = .a
CONFIG=release
endif
LIB_OSOA=$(LIB_OSOA_BASE)$(LIB_OSOA_SUFFIX)

OBJ_BASE=lib
OBJ_DIR = $(OBJ_BASE)/$(CONFIG)
BIN_DIR = bin

SRCS := $(foreach prj, $(PRJS), $(wildcard $(prj)/*.cc))
OBJS := $(foreach prj, $(PRJS), $(addprefix $(OBJ_DIR)/, $(patsubst %.cc, %.o, $(subst $(prj)/,,$(wildcard $(prj)/*.cc)))))

VPATH := $(addsuffix :, $(PRJS))
INC := $(addprefix -I, $(PRJS))
INC += -I$(BOOST_DIR)

$(OBJ_DIR)/%.o : %.cc 
	$(COMPILE.cc) $(INC) $(OUTPUT_OPTION) $<

all: $(BIN_DIR)/test $(OBJ_DIR)/$(LIB_OSOA)

debug: CXXFLAGS += -DDEBUG -g -O0
debug: $(BIN_DIR)/test $(OBJ_DIR)/$(LIB_OSOA)

ifneq ($(MAKECMDGOALS), debug)
CXXFLAGS += -O3 -DNDEBUG
endif

ifeq ($(MAKECMDGOALS),clean)
DEPS=
else
DEPS=$(OBJS:.o=.d)
$(OBJ_DIR)/%.d : %.cc
	$(CXX) $(CXXFLAGS) -MM $(INC) $< |sed -e '1 s/^/obj\//' > $@ 
-include $(DEPS)
endif

$(BIN_DIR)/test: $(OBJS)
	$(LINK.cc) $(OBJS) -dynamic -pthread -lboost_program_options -lboost_log_setup -lboost_log -lboost_system -lboost_thread -lboost_date_time -lboost_filesystem -lboost_chrono $(OUTPUT_OPTION)
	ctags -R --c-kinds=+cdefglmnpstuvx --extra=+f
	cscope -Rb

$(OBJ_DIR)/$(LIB_OSOA): $(OBJS)
	mkdir -p $(OBJ_DIR)
	ar cr $@ $(OBJS)

$(OBJS) $(DEPS) : | $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: clean
clean :
	rm -f output.log
	rm -rf $(OBJ_BASE) $(BIN_DIR) *.o *.d
