# --- Configuración de Versión ---
VERSION_MAJOR = 1
VERSION_MINOR = 0
VERSION_PATCH = 0
VERSION_STR   = "$(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH)"

# --- Directorios Fuente ---
MAPS_SRC_DIR = ./dev/maps
RES_SRC_DIR  = ./res
SRC_DIR      = ./src
TARGET_BASE  = mi_programa
TARGET       = $(TARGET_BASE)_v$(VERSION_MAJOR).$(VERSION_MINOR)

# Herramientas
CC       = gcc
TMX2BIN  = tmx2bin

# --- Reglas de Selección ---
.PHONY: all debug release clean directories copy_assets

all: debug

debug: CFLAGS = -Wall -g -DDEBUG
debug: BUILD_DIR = ./debug
debug: execute_build

release: CFLAGS = -Wall -O3 -s
release: BUILD_DIR = ./release
release: execute_build

# --- Lógica de Construcción ---
execute_build:
	$(MAKE) build_process BUILD_DIR=$(BUILD_DIR) CFLAGS="$(CFLAGS) -DVERSION_STR='$(VERSION_STR)'"

# El proceso ahora incluye la copia de assets
build_process: directories copy_assets $(BIN_FILES) $(TARGET)

OBJ_DIR      = $(BUILD_DIR)/obj
MAPS_OUT_DIR = $(BUILD_DIR)/res/maps
RES_DEST_DIR = $(BUILD_DIR)/res

# Listado de archivos
TMX_FILES = $(wildcard $(MAPS_SRC_DIR)/*.tmx)
BIN_FILES = $(patsubst $(MAPS_SRC_DIR)/%.tmx, $(MAPS_OUT_DIR)/%.bin, $(TMX_FILES))
C_FILES   = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_FILES))

# --- Recetas ---

$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $(BUILD_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Conversión de mapas
$(MAPS_OUT_DIR)/%.bin: $(MAPS_SRC_DIR)/%.tmx
	@echo "Convirtiendo mapa: $<"
	$(TMX2BIN) $< $@

# COPIA DE ASSETS: Copia todo lo de ./res al destino, excepto lo que ya manejamos
copy_assets:
	@echo "Sincronizando recursos en $(RES_DEST_DIR)..."
	@mkdir -p $(RES_DEST_DIR)
	@cp -rn $(RES_SRC_DIR)/* $(RES_DEST_DIR) 2>/dev/null || true

directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(MAPS_OUT_DIR)

clean:
	rm -rf ./debug ./release