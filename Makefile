PROJECT = actio

GRAPHIC_API=OPENGL
MKDIR_P=mkdir -p
PHYSX_LIB=CHECKED
PHYSX_ARCH=x64
BUILD_TYPE=debug
OPTIMIZE_LEVEL=0
RESOURCE_DIR='"./resources"'

SRCDIR :=src
OBJDIR :=obj

SRCS			:=$(shell find $(SRCDIR) -name "*.cpp")
OBJS			:=$(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
OBJ_TREE 		:=$(shell find $(SRCDIR) -type d)
OBJ_TREE 		:=$(patsubst src%,obj%,$(OBJ_TREE))
OBJS			:=$(patsubst %/%,%_%,$(OBJS))
CPPFLAGS		=$(@:$(OBJDIR)/%.o)-DGRAPHIC_API_$(GRAPHIC_API) -DGLM_ENABLE_EXPERIMENTAL -DGLFW_INCLUDE_NONE -DRESOURCE_DIR=$(RESOURCE_DIR) -std=c++11 -pipe -O$(OPTIMIZE_LEVEL)
LDFLAGS_COMPILE	=\
				-I/usr/include \
				-Ivendor/ \
				-Ivendor/glm \
				-Ivendor/assimp/include \
				-Ivendor/soil/src \
				-Ivendor/DevIL/DevIL/include/ \
				-Ivendor/glfw/include \
				-Ivendor/glad/include \
				-Ivendor/imgui \
				-Ivendor/boost_1_65_1 \
				-Ivendor/PhysX-3.4/PhysX_3.4/Include \
				-Ivendor/PhysX-3.4/PxShared/include \
				-Ivendor/ozz-animation/include \
				-Ivendor/entityx
LDFLAGS_BUILD  	=\
				-L./libs/linux64 \
				./bin/linux64/libSOIL.a \
				./bin/linux64/libiniparser.a \
				./bin/linux64/libimgui.a \
				./bin/linux64/libPhysX3Extensions$(PHYSX_LIB).a \
				./bin/linux64/libozz_animation_offline.a \
				./bin/linux64/libozz_animation.a \
				./bin/linux64/libozz_base.a \
				./bin/linux64/libozz_geometry.a \
				./bin/linux64/libozz_options.a \
				./bin/linux64/libozz_animation_offline_anim_tools.a \
				./bin/linux64/libozz_animation_offline_skel_tools.a \
				-Wl,-z,origin \
				-Wl,-rpath='libs/linux64' \
				-Wl,-rpath-link='libs/linux64' \
				-Wl,-Bstatic -pthread \
				-Wl,-Bdynamic \
					-lX11 -ldl -lGL -lz -lglfw \
					-ljasper -lIL -lILU -lILUT \
					-lassimp -lentityx \
					-lPhysX3Common$(PHYSX_LIB)_$(PHYSX_ARCH) -lPhysX3Gpu$(PHYSX_LIB)_$(PHYSX_ARCH) -lPhysX3$(PHYSX_LIB)_$(PHYSX_ARCH) -lPhysX3Cooking$(PHYSX_LIB)_$(PHYSX_ARCH) -lPxFoundation$(PHYSX_LIB)_$(PHYSX_ARCH) -lPxPvdSDK$(PHYSX_LIB)_$(PHYSX_ARCH) -lPhysX3CharacterKinematic$(PHYSX_LIB)_$(PHYSX_ARCH) \
					-lboost_thread -lboost_system -lboost_timer -lboost_chrono -lboost_date_time -lboost_filesystem -lboost_regex

DEPS = $(OBJS:.o=.d)
-include $(DEPS)

.PHONY: all clean tree

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "CC [$<]"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS)$(LDFLAGS_COMPILE) -MMD -MF $(patsubst %.o,%.d,$@) -o $@ -c $<	

#fix, release skipping
all: debug release
release: clean tree build_release program
debug: tree build_debug program

build_release:
	OPTIMIZE_LEVEL=2
	PHYSX_LIB=
	BUILD_TYPE=release

build_debug:
	OPTIMIZE_LEVEL=0
	PHYSX_LIB=CHECKED
	BUILD_TYPE=debug

program: $(OBJS)
	@echo "Linking..."
	@$(CXX) -Wall -Wl,--as-needed $^ -o build/$(BUILD_TYPE)/$(PROJECT) $(LDFLAGS_BUILD)
	@echo "Done."

tree:
	@$(MKDIR_P) build/$(BUILD_TYPE)/
	@$(MKDIR_P) $(OBJ_TREE)

clean: remove_objects tree

remove_objects:
	$(RM) -r $(OBJDIR)