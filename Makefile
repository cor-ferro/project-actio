PROJECT = actio

GRAPHIC_API=OPENGL
MKDIR_P=mkdir -p

SRCDIR :=src
OBJDIR :=obj

SRCS			:=$(shell find $(SRCDIR) -name "*.cpp")
OBJS			:=$(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
OBJ_TREE 		:=$(shell find $(SRCDIR) -type d)
OBJ_TREE 		:=$(patsubst src%,obj%,$(OBJ_TREE))
OBJS			:=$(patsubst %/%,%_%,$(OBJS))
CPPFLAGS		=$(@:$(OBJDIR)/%.o)-DGRAPHIC_API_$(GRAPHIC_API) -std=c++11 -pipe -pthread -O0 
LDFLAGS_COMPILE	=-Ivendor/ -Ivendor/assimp/include -Ivendor/soil/src -Ivendor/DevIL/DevIL/include/
LDFLAGS_BUILD  	=\
				-Lvendor/boost_1_65_0/ \
				-Lvendor/assimp/lib \
				-Lvendor/entityx/ \
				-Lvendor/DevIL/DevIL/build/lib/x64 \
				vendor/soil/lib/libSOIL.a \
				vendor/iniparser/libiniparser.a \
				-Wl,-Bstatic -lassimp -lentityx -lSOIL -lboost_thread -lboost_system -lboost_timer -lboost_chrono -lboost_date_time -lboost_filesystem -lboost_regex -pthread \
				-Wl,-Bdynamic -lglut -lGL -lGLU -lGLEW -lz -lIL -lILU -lILUT

DEPS = $(OBJS:.o=.d)
-include $(DEPS)

BUILD_TYPE=debug

.PHONY: all clean tree

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CPPFLAGS)$(CXXFLAGS)$(LDFLAGS_COMPILE) -MMD -MF $(patsubst %.o,%.d,$@) -o $@ -c $<

#fix, release skipping
all: debug release
release: tree build_release program
debug: tree build_debug program

build_release:
	BUILD_TYPE=release

build_debug:
	BUILD_TYPE=debug

program: $(OBJS)
	$(CXX) -Wall -Wl,--as-needed $^ -o build/$(BUILD_TYPE)/$(PROJECT) $(LDFLAGS_BUILD)

tree:
	@$(MKDIR_P) build/$(BUILD_TYPE)/
	@$(MKDIR_P) $(OBJ_TREE)

clean: remove_objects tree

remove_objects:
	$(RM) -r $(OBJDIR)

setup:
	@export LD_LIBRARY_PATH=./vendor/assimp/lib:./vendor/entityx