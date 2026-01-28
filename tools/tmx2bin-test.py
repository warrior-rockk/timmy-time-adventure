import xml.etree.ElementTree as ET
import struct
import base64
import zlib
import sys
import os

def get_custom_properties(element):
    """Extrae propiedades personalizadas de un nodo XML de Tiled."""
    props = {}
    properties_node = element.find('properties')
    if properties_node is not None:
        for prop in properties_node.findall('property'):
            name = prop.attrib.get('name')
            value = prop.attrib.get('value')
            # Intentamos convertir a int si es posible, si no, lo dejamos como string
            try:
                props[name] = int(value)
            except (ValueError, TypeError):
                props[name] = value
    return props

def parse_tmx_and_write_binary(tmx_file, bin_file):
    if not os.path.exists(tmx_file):
        print(f"Error: file {tmx_file} doesn't exist")
        return

    try:
        tree = ET.parse(tmx_file)
        root = tree.getroot()

        # 1. Atributos Básicos
        map_width = int(root.attrib.get('width'))
        map_height = int(root.attrib.get('height'))
        tile_width = int(root.attrib.get('tilewidth'))
        tile_height = int(root.attrib.get('tileheight'))
        
        # Propiedades del mapa (ej: "musica", "dificultad")
        map_props = get_custom_properties(root)

        layers = root.findall('layer')
        
        with open(bin_file, 'wb') as f:            
            # --- HEADER (12 bytes) ---
            # H: uint16 (2 bytes)
            # Agregamos 'num_props' para saber cuántas propiedades leer después
            header = struct.pack('<HHHHHH', 
                                 tile_width, tile_height, 
                                 map_width, map_height, 
                                 len(layers), len(map_props))
            f.write(header)

            # --- SECCIÓN: PROPIEDADES DEL MAPA ---
            # Guardamos cada propiedad como: [ID_Prop (B)][Valor (H)]
            # Nota: Esto es un ejemplo simple para valores numéricos
            for i, (name, value) in enumerate(map_props.items()):
                if isinstance(value, int):
                    f.write(struct.pack('<HH', i, value)) 

            # --- SECCIÓN: CAPAS ---
            for layer in layers:
                data_node = layer.find('data')
                encoding = data_node.attrib.get('encoding')
                compression = data_node.attrib.get('compression')

                # Decodificación
                if encoding == 'csv':
                    data_text = data_node.text.strip()
                    tiles = [int(x) for x in data_text.replace('\n', '').split(',') if x]        
                elif encoding == 'base64':
                    raw_data = base64.b64decode(data_node.text.strip())
                    if compression == 'zlib':
                        raw_data = zlib.decompress(raw_data)
                    total_tiles = len(raw_data) // 4
                    tiles = list(struct.unpack(f'<{total_tiles}I', raw_data))
                
                # Escribir capa (1 byte por tile)
                f.write(struct.pack(f'<{len(tiles)}B', *[t & 0xFF for t in tiles]))

        print(f"--- Exportación Exitosa ---")
        print(f"Capas procesadas: {len(layers)}")
        print(f"Propiedades encontradas: {list(map_props.keys())}")
        print(f"Tamaño final: {os.path.getsize(bin_file)} bytes")

    except Exception as e:
        print(f"Error inesperado: {e}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Uso: python tmx_to_bin.py <mapa.tmx> <salida.bin>")
    else:
        parse_tmx_and_write_binary(sys.argv[1], sys.argv[2])