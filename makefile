CXXFLAGS = -DBOOST_ALL_DYN_LINK
CXXFLAGS += -Wall -Wextra -ansi -pedantic -Weffc++ -Wshadow -Werror
#CXXFLAGS += -Wno-error=c++0x-compat
CXXFLAGS += -std=c++11

ITEM = item
SERVICE = service
UTIL = util
TEST = test
BASE = .

PRJS := $(ITEM) $(SERVICE) $(UTIL) $(BASE) $(TEST)

OBJDIR = lib
BIN_DIR = bin

SRCS := $(foreach prj, $(PRJS), $(wildcard $(prj)/*.cc))
OBJS := $(foreach prj, $(PRJS), $(addprefix $(OBJDIR)/, $(patsubst %.cc, %.o, $(subst $(prj)/,,$(wildcard $(prj)/*.cc)))))

VPATH := $(addsuffix :, $(PRJS))
INC := $(addprefix -I, $(PRJS))
INC += -I$(BOOST_DIR)

$(OBJDIR)/%.o : %.cc 
	$(COMPILE.cc) $(INC) $(OUTPUT_OPTION) $<

all: $(BIN_DIR)/test 

debug: CXXFLAGS += -DDEBUG -g -O0
debug: $(BIN_DIR)/test

ifneq ($(MAKECMDGOALS), debug)
CXXFLAGS += -O3
endif

ifeq ($(MAKECMDGOALS),clean)
DEPS=
else
DEPS=$(OBJS:.o=.d)
$(OBJDIR)/%.d : %.cc
	$(CXX) $(CXXFLAGS) -MM $(INC) $< |sed -e '1 s/^/obj\//' > $@ 
-include $(DEPS)
endif

$(BIN_DIR)/test: $(OBJS)
	$(LINK.cc) $(OBJS) -dynamic -pthread -lboost_program_options -lboost_log_setup -lboost_log -lboost_system -lboost_thread -lboost_date_time -lboost_filesystem $(OUTPUT_OPTION)
ctags -R --c-kinds=+cdefglmnpstuvx --extra=+f

$(OBJS) $(DEPS) : | $(OBJDIR) $(BIN_DIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)

.PHONY: clean
clean :
	rm -f numbers output.log
	rm -rf $(OBJDIR) $(BIN_DIR) *.o *.d
