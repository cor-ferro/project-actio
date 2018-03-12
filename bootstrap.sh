#!/bin/bash

CURRENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DEST_LIBS_DIR=$CURRENT_DIR/libs/linux64
DEST_BIN_DIR=$CURRENT_DIR/bin/linux64
COUNT_THREADS=5
PHYSX_BUILD_TYPE=checked

init_git_module()
{
	local previousDir=$(pwd)
	
	cd $CURRENT_DIR
	git submodule init $1
	git submodule update $1
	cd $previousDir
}

build_dependencies()
{
	create_libs_dir	

	build_physx
	build_boost
	build_assimp
	build_entityx
	build_glm
	build_glfw
	build_glad
	build_imgui
	build_iniparser
	build_ozz_animation
	build_soil
	build_devIL
	build_jasper
}

create_libs_dir()
{
	mkdir -p $DEST_LIBS_DIR
}

copy_to_libs()
{
	local pattern=$1
	cp -fva $pattern $DEST_LIBS_DIR 2>/dev/null
}

copy_to_bin()
{
	local pattern=$1
	cp -fva $pattern $DEST_BIN_DIR
}

build_assimp()
{
	init_git_module "vendor/assimp"

	cd $CURRENT_DIR/vendor/assimp
	rm -rf ./build
	mkdir -p build
	cd build
	cmake .. -G 'Unix Makefiles' -U=* -DASSIMP_LIB_INSTALL_DIR=$DEST_LIBS_DIR -DASSIMP_NO_EXPORT=ON -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF -DASSIMP_BUILD_BLEND_IMPORTER=ON -DASSIMP_BUILD_FBX_IMPORTER=ON -DASSIMP_BUILD_OBJ_IMPORTER=ON -DASSIMP_BUILD_TESTS=OFF
	make -j$COUNT_THREADS
	copy_to_libs "./code/libassimp*.so*"
	copy_to_bin "./contrib/irrXML/libIrrXML.a"
	cd ..
	rm -rf ./build
}

build_entityx()
{
	init_git_module "vendor/entityx"

	cd $CURRENT_DIR/vendor/entityx
	mkdir -p build
	cd build
	cmake .. -G 'Unix Makefiles' -U=* -DENTITYX_BUILD_SHARED=1 -DENTITYX_BUILD_TESTING=0
	make -j$COUNT_THREADS
	copy_to_libs "./*.so*"
	cd ..
	rm -rf ./build
}

build_physx()
{
	init_git_module "vendor/PhysX-3.4"

	cd $CURRENT_DIR/vendor/PhysX-3.4/PhysX_3.4/Source/compiler/linux64
	make clean
	make $PHYSX_BUILD_TYPE -j$COUNT_THREADS

	cd $CURRENT_DIR/vendor/PhysX-3.4

	copy_to_bin "./PhysX_3.4/Lib/linux64/*.a"
	copy_to_libs "./PhysX_3.4/Bin/linux64/*.so"
	copy_to_bin "./PxShared/lib/linux64/*.a"
	copy_to_libs "./PxShared/bin/linux64/*.so"

	cd $CURRENT_DIR/vendor/PhysX-3.4/PhysX_3.4/Source/compiler/linux64
	make clean
}

build_boost()
{
	echo "build_boost"

	cd $CURRENT_DIR/vendor

	local version="1.65.1"
	local file_name="boost_1_65_1"

	if [ ! -d ./"$file_name" ]; then
		if [ ! -f ./"$file_name.zip" ]; then
			echo "download"
			#build_boost_download $version $file_name
		else
			build_boost_validate_zip $version $file_name
		fi
	else
		echo "build libs"
		#build_boost_libs $version $file_name
	fi
}

build_boost_download()
{
	echo "build_boost_download"

	local version=$1
	local file_name=$2

	wget "https://dl.bintray.com/boostorg/release/$version/source/$file_name.zip" -c -O $file_name.zip

	build_boost_validate_zip $version $file_name
}

build_boost_validate_zip()
{
	echo "build_boost_validate_zip"

	local version=$1
	local file_name=$2

	wget "https://dl.bintray.com/boostorg/release/$version/source/$file_name.zip.json" -O $file_name.zip.json

	local sha_hash=$(grep 'sha256\".*' ./${file_name}.zip.json | grep -o '[0-9a-f]\{64\}')
	local hash_match=$(shasum -a 256 ./${file_name}.zip | grep -o "$sha_hash")
	local chars_match="${#hash_match}"

	if [ $chars_match -eq 64 ]; then
		unzip ./$file_name.zip && {
			rm -f ./$file_name.zip
			rm -f ./$file_name.zip.json
		}
		build_boost_libs $version $file_name
	else
		build_boost_download $version $file_name
	fi
}

build_boost_libs()
{
	echo "build_boost_libs"

	local version=$1
	local file_name=$2

	cd ./$file_name
	./bootstrap.sh --with-toolset=gcc --with-libraries=system,thread,timer,chrono,date_time,filesystem,regex
	./b2

	copy_to_libs "./stage/lib/*.so*"
	copy_to_bin "./stage/lib/*.a"
}

build_glm()
{
	echo "glm done"
}

build_glfw()
{
	init_git_module "vendor/glfw"

	cd $CURRENT_DIR/vendor/glfw
	mkdir -p build
	cd build
	cmake .. -G 'Unix Makefiles' -U=* -DBUILD_SHARED_LIBS=ON -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_INSTALL=OFF
	make -j$COUNT_THREADS
	copy_to_libs "./src/libglfw*.so*"
	cd ..
	rm -rf ./build
}

build_glad()
{
	echo "glad done"
}

#must be called after build glfw
build_imgui()
{
	init_git_module "vendor/imgui"

	cd $CURRENT_DIR
	mkdir -p tmp_imgui
	export LD_LIBRARY_PATH="$DEST_LIBS_DIR"

	g++ -I$CURRENT_DIR/vendor/glad/include \
		-I$CURRENT_DIR/vendor \
		-I$CURRENT_DIR/src \
		-L$DEST_LIBS_DIR \
		-c $CURRENT_DIR/vendor/imgui/imgui.cpp  \
		-o tmp_imgui/imgui.o \
		-lglfw

	g++ -I$CURRENT_DIR/vendor/glad/include \
		-I$CURRENT_DIR/vendor \
		-I$CURRENT_DIR/src \
		-L$DEST_LIBS_DIR \
		-c $CURRENT_DIR/vendor/imgui/imgui_draw.cpp  \
		-o tmp_imgui/imgui_draw.o

	ar rcs tmp_imgui/libimgui.a tmp_imgui/imgui.o tmp_imgui/imgui_draw.o
	copy_to_bin "tmp_imgui/libimgui.a"
	rm -rf tmp_imgui
}

build_iniparser()
{
	init_git_module "vendor/iniparser"

	cd $CURRENT_DIR/vendor/iniparser
	make clean
	make -j$COUNT_THREADS
	copy_to_bin "libiniparser.a"
}

build_ozz_animation()
{
	init_git_module "vendor/ozz-animation"

	cd $CURRENT_DIR/vendor/ozz-animation
	rm -rf ./build
	mkdir -p build
	cd build
	cmake .. -G 'Unix Makefiles' -U=* -Dozz_build_cpp11=ON -Dozz_build_fbx=OFF -Dozz_build_howtos=OFF -Dozz_build_msvc_rt_dll=OFF -Dozz_build_samples=OFF -Dozz_build_simd_ref=ON -Dozz_run_tests_headless=OFF
	cmake --build ./
	find . -type f -name "*.a" -exec cp -fva {} $DEST_BIN_DIR \;
	cd ..
	rm -rf ./build
}

build_soil()
{
	init_git_module "vendor/soil"

	cd $CURRENT_DIR/vendor/soil
	rm -rf ./build
	mkdir -p build
	cd build
	cmake .. -G 'Unix Makefiles' -U=*
	make -j$COUNT_THREADS
	copy_to_bin "libSOIL.a"
	cd ..
	rm -rf ./build
}

build_devIL()
{
	init_git_module "vendor/DevIL"

	cd $CURRENT_DIR/vendor/DevIL/DevIL
	rm -rf ./build
	mkdir -p build
	cd build
	cmake .. -G 'Unix Makefiles' -U=*
	make -j$COUNT_THREADS
	copy_to_libs "./lib/x64/lib*.so*"
	cd ..
	rm -rf ./build
}

build_jasper()
{
	init_git_module "vendor/jasper"

	cd $CURRENT_DIR/vendor/jasper

	rm -rf ./buildlib
	rm -rf ./install
	cmake -G "Unix Makefiles" -U=* -H./ -B./buildlib -DCMAKE_INSTALL_PREFIX=./install -DJAS_ENABLE_DOC=false
	cd buildlib
	make install -j$COUNT_THREADS
	cd ..
	copy_to_libs "./install/lib/*.so*"
	copy_to_libs "./install/lib64/*.so*"
	rm -rf ./buildlib
	rm -rf ./install
}

build_dependencies

echo ""
echo "--------------------------------------------------"
echo "finish build dependencies"