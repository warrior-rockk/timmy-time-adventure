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
APP  				:= game.exe
SRC_EXT				:= c
#project folders
SRC_DIR				:= ./src/
BUILD_DIR			:= ./build/
RESOURCES_DIR		:= ./res/
STATIC_DIR			:= ./static/
INCLUDES_DIR		:= ${OS_INC_DIR}
LIBS_DIR			:= ${OS_LIB_DIR}
DEBUG_BIN_DIR   	:= ${BUILD_DIR}debug/bin/
DEBUG_OBJS_DIR  	:= ${BUILD_DIR}debug/obj/
DEBUG_RES_DIR		:= ${DEBUG_BIN_DIR}/res/
RELEASE_BIN_DIR 	:= ${BUILD_DIR}release/bin/
RELEASE_OBJS_DIR  	:= ${BUILD_DIR}release/obj/
RELEASE_RES_DIR		:= ${RELEASE_BIN_DIR}/res/
#objects
SRCS  				:= $(wildcard ${SRC_DIR}*.${SRC_EXT})
DEBUG_OBJS 			:= $(patsubst ${SRC_DIR}%.${SRC_EXT}, ${DEBUG_OBJS_DIR}%.o, ${SRCS})
RELEASE_OBJS 		:= $(patsubst ${SRC_DIR}%.${SRC_EXT}, ${RELEASE_OBJS_DIR}%.o, ${SRCS})
#compiler/linker flags
CC					:= ${OS_GCC}
DEBUG_CFLAGS  		:= -Wall -g  -DDEBUGMODE -fgnu89-inline -I ${INCLUDES_DIR}
RELEASE_CFLAGS 		:= -Wall -O3 -fgnu89-inline -I ${INCLUDES_DIR}
LDFLAGS 			:= -fgnu89-inline -L ${LIBS_DIR} -lalleg

#map resources
MAPS_SRC_DIR = ./dev/maps
TMX_FILES = $(wildcard $(MAPS_SRC_DIR)/*.tmx)
BIN_FILES = $(patsubst $(MAPS_SRC_DIR)/%.tmx, $(DEBUG_BIN_DIR)/%.bin, $(TMX_FILES))

#dat resources
DAT_RESOURCES := ${DEBUG_BIN_DIR}game.dat ${DEBUG_BIN_DIR}player.dat ${DEBUG_BIN_DIR}coll.dat ${DEBUG_BIN_DIR}objects.dat ${DEBUG_BIN_DIR}enemies.dat ${DEBUG_BIN_DIR}jurassic.dat ${DEBUG_BIN_DIR}west.dat ${DEBUG_BIN_DIR}medieval.dat

#all targets
all: debug release

#main targets
debug: ${DEBUG_BIN_DIR}${APP} $(BIN_FILES) ${DAT_RESOURCES}
release: ${RELEASE_BIN_DIR}${APP} $(BIN_FILES) ${DAT_RESOURCES}
maps: $(BIN_FILES)
dat: ${DAT_RESOURCES}	

#generate dat files
${DEBUG_BIN_DIR}game.dat: ${RESOURCES_DIR}game/
	${DAT} create $@ --bmp $^*.bmp --wav $^*.wav --midi $^*.mid --pal-bmp $^*.pal --h ${SRC_DIR}/data/gdata.h

${DEBUG_BIN_DIR}coll.dat: ${RESOURCES_DIR}collisions/
	${DAT} create $@ --bmp $^*.bmp --wav $^*.wav --midi $^*.mid --pal-bmp $^*.pal --h ${SRC_DIR}/data/cdata.h

${DEBUG_BIN_DIR}player.dat: ${RESOURCES_DIR}player/
	${DAT} create $@ --bmp $^*.bmp --wav $^*.wav --midi $^*.mid --pal-bmp $^*.pal --h ${SRC_DIR}/data/pdata.h

${DEBUG_BIN_DIR}objects.dat: ${RESOURCES_DIR}objects/
	${DAT} create $@ --bmp $^*.bmp --wav $^*.wav --midi $^*.mid --pal-bmp $^*.pal --h ${SRC_DIR}/data/odata.h

${DEBUG_BIN_DIR}enemies.dat: ${RESOURCES_DIR}enemies/
	${DAT} create $@ --bmp $^*.bmp --wav $^*.wav --midi $^*.mid --pal-bmp $^*.pal --h ${SRC_DIR}/data/edata.h

${DEBUG_BIN_DIR}jurassic.dat: ${RESOURCES_DIR}levels/jurassic/
	${DAT} create $@ --bmp $^*.bmp --wav $^*.wav --midi $^*.mid --pal-bmp $^*.pal --h ${SRC_DIR}/data/judata.h

${DEBUG_BIN_DIR}west.dat: ${RESOURCES_DIR}levels/west/
	${DAT} create $@ --bmp $^*.bmp --wav $^*.wav --midi $^*.mid --pal-bmp $^*.pal --h ${SRC_DIR}/data/wedata.h

${DEBUG_BIN_DIR}medieval.dat: ${RESOURCES_DIR}levels/medieval/
	${DAT} create $@ --bmp $^*.bmp --wav $^*.wav --midi $^*.mid --pal-bmp $^*.pal --h ${SRC_DIR}/data/medata.h

#binary target (debug)
${DEBUG_BIN_DIR}${APP}: ${DEBUG_OBJS} 
	mkdir -p ${DEBUG_BIN_DIR}	
	
	@echo "## Linking Debug ${APP}"
	${CC} $^ -o $@ ${DEBUG_CFLAGS} ${LDFLAGS}
	
	@echo "## Copy static files"
	cp -r ${STATIC_DIR}/*.* ${DEBUG_BIN_DIR}
	
#compile objects generating dependency files (debug)
${DEBUG_OBJS_DIR}%.o: ${SRC_DIR}%.${SRC_EXT}
	mkdir -p ${DEBUG_OBJS_DIR}	
	${CC} -x c -c -MD $< -o $@ ${DEBUG_CFLAGS}

# Convert .tmx to .bin
$(DEBUG_BIN_DIR)/%.bin: $(MAPS_SRC_DIR)/%.tmx | $(DEBUG_BIN_DIR)
	@echo "## Converting map: $< -> $@"
	python3 ./tools/tmx2bin.py $< $@

#binary target (release)
${RELEASE_BIN_DIR}${APP}: ${RELEASE_OBJS}
	mkdir -p ${RELEASE_BIN_DIR}

	@echo "## Linking Release ${APP}"
	${CC} $^ -o $@ ${RELEASE_CFLAGS} ${LDFLAGS}

	@echo "## Copy static files"
	cp -r ${STATIC_DIR}/*.* ${RELEASE_BIN_DIR}
	
#compile objects generating dependency files (release)
${RELEASE_OBJS_DIR}%.o: ${SRC_DIR}%.${SRC_EXT}
	mkdir -p ${RELEASE_OBJS_DIR}
	${CC} -x c -c -MD $< -o $@ ${RELEASE_CFLAGS}

#dependency includes
-include ${DEBUG_OBJS_DIR}*.d
-include ${RELEASE_OBJS_DIR}*.d

#run targets
run_debug: debug
	cd ${DEBUG_BIN_DIR} && ${OS_DOSBOX} ${APP} --fastbioslogo -conf dosbox.conf -exit -noconsole
run_debug_stop: debug
	cd ${DEBUG_BIN_DIR} && ${OS_DOSBOX} ${APP} --fastbioslogo -conf dosbox.conf -noconsole
run_release: release
	cd ${RELEASE_BIN_DIR} && ${OS_DOSBOX} ${APP} --fastbioslogo -conf dosbox.conf -exit -noconsole

.PHONY: clean info

clean:
	rm -rvf ${BUILD_DIR}

info:	
	@echo "Operating system:"
	@echo ${OS}
	@echo "Src objects:"
	@echo "${SRCS}"
	@echo "Resources:"
	@echo "${RESOURCES}"
	