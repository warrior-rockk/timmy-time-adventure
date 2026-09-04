#recursive search functions trought folders (don't use if resources are on root folder)
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

#detect SO
ifeq ($(OS),Windows_NT)
	OS := Windows_NT
	OS_INC_DIR := 'D:/Instalables/Proyectos/Programacion DOS/cross-compile/djgpp/include'
	OS_GCC	   := 'D:/Instalables/Proyectos/Programacion DOS/cross-compile/djgpp/bin/i586-pc-msdosdjgpp-gcc'	
	OS_LIB_DIR := ./
	#OS_DOSBOX  := 'D:/Instalables/Proyectos/Old Days/bin/DOSBox/DosBox.exe'
	OS_DOSBOX  := 'D:/Instalables/Proyectos/DosBox-x/dosbox-x.exe'
	DAT        := ./tools/dat/dat_win.exe
else
	UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        OS := Linux
    endif
    ifeq ($(UNAME_S),Darwin)
        OS := Mac
		OS_INC_DIR := /Users/warrior/Downloads/allegro-xc/allegro-4.2.2-xc/include
		OS_GCC	   := /Users/warrior/Downloads/djgpp/bin/i586-pc-msdosdjgpp-gcc	
		OS_LIB_DIR := /Users/warrior/Downloads/allegro-xc/allegro-4.2.2-xc/lib/djgpp
		OS_DOSBOX  := /Applications/dosbox-x.app/Contents/MacOS/dosbox-x
		DAT        := ./tools/dat/dat
    endif
endif

#project definition
APP  				:= timmy.exe
APP_TITLE			:= Timmy Time Adventure
MAJOR_VERSION		:= 0
MINOR_VERSION		:= 8
SRC_EXT				:= c
#project folders
SRC_DIR				:= ./src/
BUILD_DIR			:= ./build/
RESOURCES_DIR		:= ./res/
STATIC_DIR			:= ./static/
INCLUDES_DIR		:= ${OS_INC_DIR}
LIBS_DIR			:= ${OS_LIB_DIR}
WEB_DIR				:= ./web
#objects
SRCS  				:= $(wildcard ${SRC_DIR}*.${SRC_EXT})
C_FILES             = $(wildcard $(SRC_DIR)/*.${SRC_EXT})
MAPS_SRC_DIR 		= ${RESOURCES_DIR}maps
TMX_FILES 			= $(wildcard $(MAPS_SRC_DIR)/*.tmx)
#compiler/linker flags
CC					:= ${OS_GCC} -DMAJOR_VERSION=$(MAJOR_VERSION) -DMINOR_VERSION=$(MINOR_VERSION)
DEBUG_CFLAGS  		:= -Wall -g  -DDEBUGMODE -fgnu89-inline -I ${INCLUDES_DIR}
RELEASE_CFLAGS 		:= -Wall -O3 -fgnu89-inline -I ${INCLUDES_DIR} #not use O3 with djgpp?
LDFLAGS 			:= -fgnu89-inline -L ${LIBS_DIR} -lalleg

#all targets
all: debug release

#main target definitions
debug: CFLAGS = ${DEBUG_CFLAGS}
debug: BUILD_DIR = ./build/debug/
debug: execute_build
release: CFLAGS = ${RELEASE_CFLAGS}
release: BUILD_DIR = ./build/release/
release: execute_build
release_pack: BUILD_DIR = ./build/release/

#main make build
execute_build:
	$(MAKE) build_process BUILD_DIR=$(BUILD_DIR) CFLAGS="$(CFLAGS)"
build_process: directories ${BUILD_DIR}bin/$(APP) maps dat

#definitions depending build target
OBJ_DIR      	= $(BUILD_DIR)obj
OBJ_FILES 		= $(patsubst $(SRC_DIR)/%.${SRC_EXT}, $(OBJ_DIR)/%.o, $(C_FILES))
#map resources
BIN_FILES 		= $(patsubst $(MAPS_SRC_DIR)/%.tmx, $(BUILD_DIR)bin/%.bin, $(TMX_FILES))
#dat resources
DAT_RESOURCES := ${BUILD_DIR}bin/game.dat ${BUILD_DIR}bin/player.dat ${BUILD_DIR}bin/collmap.dat ${BUILD_DIR}bin/objects.dat ${BUILD_DIR}bin/enemies.dat ${BUILD_DIR}bin/jurassic.dat ${BUILD_DIR}bin/west.dat ${BUILD_DIR}bin/medieval.dat ${BUILD_DIR}bin/egypt.dat ${BUILD_DIR}bin/tutorial.dat

#resource targets
maps: $(BIN_FILES)
dat: ${DAT_RESOURCES}	

#binary target
${BUILD_DIR}bin/$(APP): $(OBJ_FILES)
	@echo "## Linking ${APP}"
	${CC} $(OBJ_FILES) -o $(BUILD_DIR)bin/$(APP) ${CFLAGS} ${LDFLAGS}

#compile objects generating dependency files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	${CC} -x c -c -MD $< -o $@ ${CFLAGS}

#create directories and copy static files	
directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p ${BUILD_DIR}bin
	@echo "## Copy static files"
	cp -r ${STATIC_DIR}/*.* ${BUILD_DIR}bin/

#generate dat files
${BUILD_DIR}bin/game.dat: $(wildcard ${RESOURCES_DIR}game/*)
	${DAT} create $@ --bmp ${RESOURCES_DIR}game/*.bmp --wav ${RESOURCES_DIR}game/*.wav --midi ${RESOURCES_DIR}game/*.mid --pal-bmp ${RESOURCES_DIR}game/*.pal --data ${RESOURCES_DIR}game/*.txt --h ${SRC_DIR}/data/gdata.h

${BUILD_DIR}bin/collmap.dat: $(wildcard ${RESOURCES_DIR}collisions/*)
	${DAT} create $@ --bmp ${RESOURCES_DIR}collisions/*.bmp --wav ${RESOURCES_DIR}collisions/*.wav --midi ${RESOURCES_DIR}collisions/*.mid --pal-bmp ${RESOURCES_DIR}collisions/*.pal --h ${SRC_DIR}/data/cdata.h

${BUILD_DIR}bin/player.dat: $(wildcard ${RESOURCES_DIR}player/*)
	${DAT} create $@ --bmp ${RESOURCES_DIR}player/*.bmp --wav ${RESOURCES_DIR}player/*.wav --midi ${RESOURCES_DIR}player/*.mid --pal-bmp ${RESOURCES_DIR}player/*.pal --h ${SRC_DIR}/data/pdata.h

${BUILD_DIR}bin/objects.dat: $(wildcard ${RESOURCES_DIR}objects/*)
	${DAT} create $@ --bmp ${RESOURCES_DIR}objects/*.bmp --wav ${RESOURCES_DIR}objects/*.wav --midi ${RESOURCES_DIR}objects/*.mid --pal-bmp ${RESOURCES_DIR}objects/*.pal --h ${SRC_DIR}/data/odata.h

${BUILD_DIR}bin/enemies.dat: $(wildcard ${RESOURCES_DIR}enemies/*)
	${DAT} create $@ --bmp ${RESOURCES_DIR}enemies/*.bmp --wav ${RESOURCES_DIR}enemies/*.wav --midi ${RESOURCES_DIR}enemies/*.mid --pal-bmp ${RESOURCES_DIR}enemies/*.pal --h ${SRC_DIR}/data/edata.h

${BUILD_DIR}bin/jurassic.dat: $(wildcard ${RESOURCES_DIR}levels/jurassic/*)
	${DAT} create $@ --bmp ${RESOURCES_DIR}levels/jurassic/*.bmp --wav ${RESOURCES_DIR}levels/jurassic/*.wav --midi ${RESOURCES_DIR}levels/jurassic/*.mid --pal-bmp ${RESOURCES_DIR}levels/jurassic/*.pal --h ${SRC_DIR}/data/judata.h

${BUILD_DIR}bin/west.dat: $(wildcard ${RESOURCES_DIR}levels/west/*)
	${DAT} create $@ --bmp ${RESOURCES_DIR}levels/west/*.bmp --wav ${RESOURCES_DIR}levels/west/*.wav --midi ${RESOURCES_DIR}levels/west/*.mid --pal-bmp ${RESOURCES_DIR}levels/west/*.pal --h ${SRC_DIR}/data/wedata.h

${BUILD_DIR}bin/medieval.dat: $(wildcard ${RESOURCES_DIR}levels/medieval/*)
	${DAT} create $@ --bmp ${RESOURCES_DIR}levels/medieval/*.bmp --wav ${RESOURCES_DIR}levels/medieval/*.wav --midi ${RESOURCES_DIR}levels/medieval/*.mid --pal-bmp ${RESOURCES_DIR}levels/medieval/*.pal --h ${SRC_DIR}/data/medata.h

${BUILD_DIR}bin/egypt.dat: $(wildcard ${RESOURCES_DIR}levels/egypt/*)
	${DAT} create $@ --bmp ${RESOURCES_DIR}levels/egypt/*.bmp --wav ${RESOURCES_DIR}levels/egypt/*.wav --midi ${RESOURCES_DIR}levels/egypt/*.mid --pal-bmp ${RESOURCES_DIR}levels/egypt/*.pal --h ${SRC_DIR}/data/egydata.h

${BUILD_DIR}bin/tutorial.dat: $(wildcard ${RESOURCES_DIR}levels/tutorial/*)
	${DAT} create $@ --bmp ${RESOURCES_DIR}levels/tutorial/*.bmp --wav ${RESOURCES_DIR}levels/tutorial/*.wav --midi ${RESOURCES_DIR}levels/tutorial/*.mid --pal-bmp ${RESOURCES_DIR}levels/tutorial/*.pal --h ${SRC_DIR}/data/tutdata.h

# Convert .tmx to .bin
${BUILD_DIR}bin/%.bin: $(MAPS_SRC_DIR)/%.tmx | ${BUILD_DIR}bin/
	@echo "## Converting map: $< -> $@"
	python3 ./tools/tmx2bin.py $< $@

#dependency includes
-include ${BUILD_DIR}obj/*.d
#-include ./build/release/obj/*.d

#run targets
run_debug: debug
	cd ./build/debug/bin && ${OS_DOSBOX} ${APP} --fastbioslogo -conf dosbox.conf -exit -noconsole
run_debug_stop: debug
	cd ./build/debug/bin && ${OS_DOSBOX} ${APP} --fastbioslogo -conf dosbox.conf -noconsole
run_release: release
	cd ./build/release/bin && ${OS_DOSBOX} ${APP} --fastbioslogo -conf dosbox.conf -exit -noconsole

#pack release zip
release_pack: release
	@echo "# Packing release"
ifeq ($(OS),Windows_NT)
	powershell -Command "Compress-Archive -Path ${BUILD_DIR}/bin/*.* -DestinationPath  ${BUILD_DIR}/'${APP_TITLE} (v${MAJOR_VERSION}.${MINOR_VERSION}).zip' -Force"
endif

#pack web version with jsdos emulator and run on browser
web: release
	@echo "# clean web release"
	rm -rvf ${WEB_DIR}/release
	
	mkdir ${WEB_DIR}/release
	
	@echo "# Copying html resources"
	cp ${WEB_DIR}/html/*.* ${WEB_DIR}/release
	
	@echo "# Packing .jsdos file"
ifeq ($(OS),Windows_NT)
	powershell -Command "Compress-Archive -Path ./build/release/bin/*.* -DestinationPath ${WEB_DIR}/release/$(basename $(APP)).zip -Force"
	powershell -Command "Compress-Archive -Path ${WEB_DIR}/.jsdos/ -Update ${WEB_DIR}/release/$(basename $(APP)).zip"	
	powershell -Command "Rename-Item -Path '${WEB_DIR}/release/$(basename $(APP)).zip' -NewName $(basename $(APP)).jsdos"

	@echo "# pack the web release on zip for itch.io"	
	powershell -Command "Compress-Archive -Path ${WEB_DIR}/release/*.* -DestinationPath ${WEB_DIR}/release/'${APP_TITLE} (v${MAJOR_VERSION}.${MINOR_VERSION}).zip' -Force"
endif

	@echo "# Open and run web release"
	python -c "import webbrowser; webbrowser.open('http://localhost:8000/$(basename $(notdir $(WEB_DIR)))/release/')"
	python -m http.server

.PHONY: clean info web release_pack

clean:
	rm -rvf ${BUILD_DIR}

info:	
	@echo "Operating system:"
	@echo ${OS}
	@echo "Src objects:"
	@echo "${SRCS}"	