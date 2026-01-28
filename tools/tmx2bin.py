#********************************************************************
# tmx2bin - Tool to convert txm Tiled maps on binary file
#
# 25/01/2026
# Warcom Soft. - warrior.rockk@gmail.com
#*******************************************************************/

import xml.etree.ElementTree as ET
import struct
import base64
import zlib
import sys
import os

def parse_tmx_and_write_binary(tmx_file, bin_file):
    if not os.path.exists(tmx_file):
        print(f"Error: file {tmx_file} doesn't exists")
        return

    try:
        # Parse XML
        tree = ET.parse(tmx_file)
        root = tree.getroot()

        # Obtaing map properties
        map_width = int(root.attrib.get('width'))
        map_height = int(root.attrib.get('height'))
        tile_width = int(root.attrib.get('tilewidth'))
        tile_height = int(root.attrib.get('tileheight'))

        # echo info
        print(f"Processing map: {map_width}x{map_height} tiles")
        print(f"Tile size: {tile_width}x{tile_height} px")
        
        object_groups = root.findall('objectgroup')

        # 3. Find data layer (layer). NOTE: the script takes the first layer founded
        layer = root.find('layer')
        if layer is None:
            print("Error: Can't find any layer named ('layer') in the file map")
            return

        data_node = layer.find('data')
        encoding = data_node.attrib.get('encoding')
        compression = data_node.attrib.get('compression')

        tiles = []
        
        # Decoding the data from Tiled format
        if encoding == 'csv':
            # CSV format map
            data_text = data_node.text.strip()
            tiles = [int(x) for x in data_text.replace('\n', '').split(',') if x]        
        elif encoding == 'base64':
            raw_data = base64.b64decode(data_node.text.strip())
            if compression == 'zlib':
                raw_data = zlib.decompress(raw_data)
            # Tiled base64 siempre es uint32 interno, lo leemos y luego convertimos
            total_tiles = len(raw_data) // 4
            tiles = list(struct.unpack(f'<{total_tiles}I', raw_data))
        else:
            print(f"Error: Codificacion format '{encoding}' don't support by this script")
            return
        
        # --- VALIDACIÓN Y CONVERSIÓN ---
        # Verificamos si algún ID supera 255
        if any(t > 255 for t in tiles):
            print("¡WARNING!: some tiles ID are greater than 255 and will be truncated to uint8_t.")
        
        # Write binary file
        # Struct file:
        # [Header]
        #   - Tile Width  (uint32)
        #   - Tile Height (uint32)
        #   - Map Width   (uint32) 
        #   - Map Height  (uint32)
        # [Body]
        #   - Tile array (uint32 * num_tiles)

        with open(bin_file, 'wb') as f:            
            # Pack the HEADER: 'H' = unsigned short (uint16_t, 2 bytes)
            # 8 bytes total header
            header = struct.pack('<HHHH', tile_width, tile_height, map_width, map_height)
            f.write(header)

            # Write tiles: 'B' = unsigned char (uint8_t, 1 byte)
            f.write(struct.pack(f'<{len(tiles)}B', *[t & 0xFF for t in tiles]))

            # 4. (EXTRA) Exportar Objetos simples si existen
            # Esto escribe: num_objetos (H) + lista de (x, y, tipo)
            for obj_group in object_groups:
                objs = obj_group.findall('object')
                f.write(struct.pack('<H', len(objs))) # Escribir cuántos objetos hay
                for obj in objs:
                    ox = int(float(obj.attrib.get('x', 0)))
                    oy = int(float(obj.attrib.get('y', 0)))
                    # Guardamos X e Y como uint16
                    f.write(struct.pack('<HH', ox, oy))

        print(f"--- Done ---")
        print(f"File saved in: {bin_file}")
        print(f"Bytes written: {os.path.getsize(bin_file)}")

    except Exception as e:
        print(f"Unexpected error: {e}")

if __name__ == "__main__":
    # commandline use
    if len(sys.argv) < 3:
        print("Use: python tmx_to_bin.py <input.tmx> <output.bin>")
    else:
        parse_tmx_and_write_binary(sys.argv[1], sys.argv[2])