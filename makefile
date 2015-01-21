#CC = distcc
#CXX = distcc g++
CXXFLAGS = -DBOOST_ALL_DYN_LINK
CXXFLAGS += -Wall -Wextra -ansi -pedantic -Werror #-Wfatal-errors
CXXFLAGS += -Weffc++ -Wshadow # // Disabled for Poco 
#CXXFLAGS += -Wno-error=effc++ # for boost and other libs.
CXXFLAGS += -std=c++11

ITEM = item
SERVICE = service
COMMS = $(SERVICE)/comms
UTIL = util
BASE = .

PRJS := $(ITEM) $(SERVICE) $(COMMS) $(UTIL) $(BASE)

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

INC+=-I$(POCO_BASE)/CppUnit/include
INC+=-I$(POCO_BASE)/Foundation/include
INC+=-I$(POCO_BASE)/XML/include
INC+=-I$(POCO_BASE)/Util/include
INC+=-I$(POCO_BASE)/Net/include

DEF+=-D_XOPEN_SOURCE=500
DEF+=-D_REENTRANT
DEF+=-D_THREAD_SAFE
DEF+=-D_FILE_OFFSET_BITS=64
DEF+=-D_LARGEFILE64_SOURCE
DEF+=-DPOCO_HAVE_FD_EPOLL

$(OBJ_DIR)/%.o : %.cc 
	$(COMPILE.cc) $(INC) $(DEFS) $(OUTPUT_OPTION) $<

all: $(OBJ_DIR)/$(LIB_OSOA)

debug: CXXFLAGS += -DDEBUG -g -O0
debug: $(OBJ_DIR)/$(LIB_OSOA)

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

#$(OBJS)
#	$(LINK.cc) $(OBJS) $(DEFS) -L$(POCO_BASE)/lib/Linux/x86_64 -dynamic -lPocoUtild -lPocoNetd -lPocoXMLd -lPocoFoundationd -pthread -lboost_program_options -lboost_log_setup -lboost_log -lboost_system -lboost_thread -lboost_date_time -lboost_filesystem -lboost_chrono $(OUTPUT_OPTION)
#	#ctags -R --c-kinds=+cdefglmnpstuvx --extra=+f
#	#cscope -Rb

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
	rm -f *.gch
