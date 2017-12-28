PROJECT = actio

GRAPHIC_API=OPENGL
MKDIR_P=mkdir -p
PHYSX_LIB=CHECKED
PHYSX_ARCH=x64
BUILD_TYPE=debug

SRCDIR :=src
OBJDIR :=obj

SRCS			:=$(shell find $(SRCDIR) -name "*.cpp")
OBJS			:=$(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
OBJ_TREE 		:=$(shell find $(SRCDIR) -type d)
OBJ_TREE 		:=$(patsubst src%,obj%,$(OBJ_TREE))
OBJS			:=$(patsubst %/%,%_%,$(OBJS))
CPPFLAGS		=$(@:$(OBJDIR)/%.o)-DGRAPHIC_API_$(GRAPHIC_API) -std=c++11 -pipe -O0
LDFLAGS_COMPILE	=-Ivendor/ \
				-Ivendor/assimp/include \
				-Ivendor/soil/src \
				-Ivendor/DevIL/DevIL/include/ \
				-Ivendor/glfw/include \
				-Ivendor/glad/include \
				-I../PhysX-3.4/PhysX_3.4/Include \
				-I../PhysX-3.4/PxShared/include
LDFLAGS_BUILD  	=\
				-L./libs/linux64 \
				./vendor/soil/lib/libSOIL.a \
				./vendor/iniparser/libiniparser.a \
				../PhysX-3.4/PhysX_3.4/Lib/linux64/libPhysX3Extensions$(PHYSX_LIB).a \
				-Wl,-rpath=./libs/linux64 \
				-Wl,-rpath-link=./libs/linux64 \
				-Wl,-Bstatic -pthread \
				-Wl,-Bdynamic \
					-lX11 -ldl -lGL -lz -lglfw \
					-lIL -lILU -lILUT \
					-lassimp -lentityx \
					-lPhysX3Common$(PHYSX_LIB)_$(PHYSX_ARCH) -lPhysX3Gpu$(PHYSX_LIB)_$(PHYSX_ARCH) -lPhysX3$(PHYSX_LIB)_$(PHYSX_ARCH) -lPhysX3Cooking$(PHYSX_LIB)_$(PHYSX_ARCH) -lPxFoundation$(PHYSX_LIB)_$(PHYSX_ARCH) -lPxPvdSDK$(PHYSX_LIB)_$(PHYSX_ARCH) \
					-lboost_thread -lboost_system -lboost_timer -lboost_chrono -lboost_date_time -lboost_filesystem -lboost_regex

DEPS = $(OBJS:.o=.d)
-include $(DEPS)

.PHONY: all clean tree

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)$(LDFLAGS_COMPILE) -MMD -MF $(patsubst %.o,%.d,$@) -o $@ -c $<

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