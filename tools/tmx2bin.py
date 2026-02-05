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

#define entity classes
entity_classes = {
     "player"   : 0,
     "object"   : 1,
     "enemy"    : 2,
     "platform" : 3,     
}
#define entity player types
player_ent_types = {
    "player1"   : 0,
    "player2"   : 1,
}
#define entity object types
object_ent_types = {
    "gem"   : 0,
    "stone" : 1,
}
#define entity enemies types
enemy_ent_types = {
    "ptero"   : 0,
    "raptor"  : 1,
}
#define array of lists of entity types
ent_types = [player_ent_types, object_ent_types, enemy_ent_types]

def get_custom_properties(element):
    """Get custom properties and returns on dictionary."""
    props = {}
    properties_node = element.find('properties')
    if properties_node is not None:
        for prop in properties_node.findall('property'):
            name = prop.attrib.get('name')
            value = prop.attrib.get('value')
            # Try to convert to int or float or string
            try:
                if '.' in value:
                    props[name] = float(value)
                else:
                    props[name] = int(value)
            except (ValueError, TypeError):
                props[name] = value
    return props

def get_property_value(element, property_name, default=None):
    """
    Gets a specific property by name on object
    Returns converted value o defect value if not exists    
    """
    properties_node = element.find('properties')
    if properties_node is not None:
        for prop in properties_node.findall('property'):
            if prop.attrib.get('name') == property_name:
                value = prop.attrib.get('value')
                # Try to convert the type 
                try:
                    if '.' in value: return float(value)
                    return int(value)
                except (ValueError, TypeError):
                    return value # return as string if not numeric
    return default

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
        
        # 3. Find data layer (layer). NOTE: the script takes the first layer founded
        layer = root.find('layer')
        if layer is None:
            print("Error: Can't find any layer named ('layer') in the file map")
            return

        backColor = get_property_value(layer, "backColor", default=0)
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
                
        # Check if any tile ID > 255
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
            header = struct.pack('<HHHHH', tile_width, tile_height, map_width, map_height, backColor)
            f.write(header)

            # Write tiles: 'B' = unsigned char (uint8_t, 1 byte)
            f.write(struct.pack(f'<{len(tiles)}B', *[t & 0xFF for t in tiles]))
            
            # Search object layer            
            for obj_group in root.findall('objectgroup'):
                layerName = obj_group.get('name')
                if layerName != 'Objects':
                    # Si la capa no está en nuestra lista, saltamos a la siguiente
                    continue
                
                # Get object data
                # Write: numObjects (H) + list of (x0, y0, dir0)
                print(f"✅ Procesando Capa: '{layerName}'")                
                
                objs = obj_group.findall('object')
                f.write(struct.pack('<H', len(objs))) # Write num objects
                print(f"📍 Processing objets: {len(objs)} objects")
                for obj in objs:                                        
                    # Get class and type (Tiled uses class or type for class by version)
                    raw_class = obj.attrib.get('class') or obj.attrib.get('type') or ""
                    raw_type = obj.attrib.get('name')                    
                    # Get from our arrays of entities class and types                    
                    entClass = entity_classes.get(raw_class, 0)
                    entType = ent_types[entClass].get(raw_type, 0)
                    # Get general entity data
                    ox = int(float(obj.attrib.get('x', 0)))
                    oy = int(float(obj.attrib.get('y', 0)))
                    oDir = get_property_value(obj, "dir", default=0)
                    
                    # write object data
                    f.write(struct.pack('<BBHHB', entClass, entType, ox, oy, oDir))                    
                    # print info
                    print(f"\tClass:{raw_class} - Type:{raw_type}")

            # Search enemies layer            
            for obj_group in root.findall('objectgroup'):
                layerName = obj_group.get('name')
                if layerName != 'Enemies':
                    # Si la capa no está en nuestra lista, saltamos a la siguiente
                    continue
                
                # Get object data
                # Write: numObjects (H) + list of (x0, y0, dir0)
                print(f"✅ Procesando Capa: '{layerName}'")                
                
                objs = obj_group.findall('object')
                f.write(struct.pack('<H', len(objs))) # Write num objects
                print(f"📍 Processing objets: {len(objs)} objects")
                for obj in objs:                                        
                    # Get class and type (Tiled uses class or type for class by version)
                    raw_class = obj.attrib.get('class') or obj.attrib.get('type') or ""
                    raw_type = obj.attrib.get('name')                    
                    # Get from our arrays of entities class and types                    
                    entClass = entity_classes.get(raw_class, 0)
                    entType = ent_types[entClass].get(raw_type, 0)
                    # Get general entity data
                    ox = int(float(obj.attrib.get('x', 0)))
                    oy = int(float(obj.attrib.get('y', 0)))
                    oDir = get_property_value(obj, "dir", default=0)
                    
                    # write object data
                    f.write(struct.pack('<BBHHB', entClass, entType, ox, oy, oDir))                    
                    # print info
                    print(f"\tClass:{raw_class} - Type:{raw_type}")

        print(f"✅--- Done ---")
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