import xml.etree.ElementTree as ET
import struct
import base64
import zlib
import sys
import os

def parse_tmx_and_write_binary(tmx_file, bin_file):
    if not os.path.exists(tmx_file):
        print(f"Error: El archivo {tmx_file} no existe.")
        return

    try:
        # 1. Parsear el XML
        tree = ET.parse(tmx_file)
        root = tree.getroot()

        # 2. Obtener propiedades básicas del mapa
        # Tiled guarda estas dimensiones en atributos del tag <map>
        map_width = int(root.attrib.get('width'))
        map_height = int(root.attrib.get('height'))
        tile_width = int(root.attrib.get('tilewidth'))
        tile_height = int(root.attrib.get('tileheight'))

        print(f"Procesando mapa: {map_width}x{map_height} tiles.")
        print(f"Tamaño de tile: {tile_width}x{tile_height} px.")

        # 3. Buscar la capa de datos (layer)
        # Nota: Este script toma la PRIMERA capa encontrada. 
        layer = root.find('layer')
        if layer is None:
            print("Error: No se encontró ninguna capa ('layer') en el archivo.")
            return

        data_node = layer.find('data')
        encoding = data_node.attrib.get('encoding')
        compression = data_node.attrib.get('compression')

        tiles = []

        # 4. Decodificar los datos según el formato de Tiled
        if encoding == 'csv':
            # Formato texto separado por comas
            data_text = data_node.text.strip()
            tiles = [int(x) for x in data_text.replace('\n', '').split(',') if x]
        
        elif encoding == 'base64':
            raw_data = base64.b64decode(data_node.text.strip())
            
            if compression == 'zlib':
                raw_data = zlib.decompress(raw_data)
            elif compression == 'gzip':
                import gzip
                raw_data = gzip.decompress(raw_data)
            
            # Convertir bytes a array de enteros (4 bytes por entero, Little Endian)
            # Tiled usa enteros de 32 bits sin signo para los IDs
            total_tiles = len(raw_data) // 4
            tiles = struct.unpack(f'<{total_tiles}I', raw_data)
        
        else:
            print(f"Error: Formato de codificación '{encoding}' no soportado por este script.")
            return

        # 5. Escribir el archivo Binario
        # Estructura elegida:
        # [Header]
        #   - Tile Width  (uint32)
        #   - Tile Height (uint32)
        #   - Map Width   (uint32) -> Necesario para saber cuándo romper la fila
        #   - Map Height  (uint32)
        # [Body]
        #   - Array de Tiles (uint32 * num_tiles)

        with open(bin_file, 'wb') as f:
            # Empaquetamos el header: 4 enteros sin signo (I), Little Endian (<)
            header = struct.pack('<IIII', tile_width, tile_height, map_width, map_height)
            f.write(header)

            # Empaquetamos el array de tiles
            # Usamos '*' para desempaquetar la lista como argumentos
            f.write(struct.pack(f'<{len(tiles)}I', *tiles))

        print(f"--- Éxito ---")
        print(f"Archivo guardado en: {bin_file}")
        print(f"Bytes escritos: {os.path.getsize(bin_file)}")

    except Exception as e:
        print(f"Ocurrió un error inesperado: {e}")

if __name__ == "__main__":
    # Uso desde línea de comandos
    if len(sys.argv) < 3:
        print("Uso: python tmx_to_bin.py <input.tmx> <output.bin>")
    else:
        parse_tmx_and_write_binary(sys.argv[1], sys.argv[2])