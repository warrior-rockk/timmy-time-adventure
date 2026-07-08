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

#define entity classes (order define entity create game order)
entity_classes = {
     "trigger"  : 0,
     "platform" : 1,     
     "player"   : 2,
     "object"   : 3,
     "enemy"    : 4,     
}
#define entity player types
player_ent_types = {
    "player1"   : 0,
    "player2"   : 1,
}
#define entity object types
object_ent_types = [
    "solid",         
    "egg",           
    "endLevel",      
    "checkpoint",    
    "item",          
    "stopScroll",    
    "wagon",        
    "dynamite",      
    "backColor",
    "bridge",    
    "rock-explosion",
    "game-over",
    "rock-fall",
    "spike-fall",
    "box",
    "hidden",
    "quick-sand",
    "trap-arrow",
    "egypt-symbol",
    "symbol-hole",
    "egypt-platform",
    "spike-trap",
    "lance",
    "scroll-mode",
    "door-in",
    "door-out",
    "spike-trap2",
    "trap-fire",
    "debug-start"   
]
object_ent_types = {name: index for index, name in enumerate(object_ent_types)}

#define entity enemies types
enemy_ent_types = [
    "ptero"         ,
    "raptor"        ,
    "spider"        ,
    "piranha"       ,
    "cowboy"        ,
    "eagle"         ,
    "bullet"        ,
    "tumble"        ,
    "trice"         ,
    "scorpion"      ,
    "indian_axe"    ,
    "axe"           ,
    "bat"           ,
    "indian_bow"    ,
    "arrow"         ,
    "egyptian"      ,
    "trap-arrow"    ,
    "beetle"        ,
    "mummy"         ,
    "hitbox"        ,
    "vulture"       ,
    "anubis"        ,
    "trap-fire"     ,
    "ghost"         ,
    "knight"        ,
    "skeleton"      ,
]
enemy_ent_types = {name: index for index, name in enumerate(enemy_ent_types)}

#define array of lists of entity types
ent_types = [object_ent_types, object_ent_types, player_ent_types, object_ent_types, enemy_ent_types]

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
        levelTime = get_property_value(root, "time", default=300)
        
        # Find data layer (layer). NOTE: the script takes the first layer founded
        layer = root.find('layer')
        if layer is None:
            print("❌Error: Can't find any layer named ('layer') in the file map")
            return

        backColor = get_property_value(layer, "backColor", default=0)
        data_node = layer.find('data')
        encoding = data_node.attrib.get('encoding')
        compression = data_node.attrib.get('compression')

        tiles = []
        tile_data = []   
        tile_animations = {}

        # Find tileset
        tileSet = root.find('tileset')
        if tileSet is None:
            print("❌Error: Can't find any tileSet embedded in the file map")
            return

        tileCount = int(tileSet.attrib.get('tilecount'))
        tileColumns = int(tileSet.attrib.get('columns'))
        
        # echo info
        print(f"Processing map: {map_width}x{map_height} tiles")
        print(f"Tile size: {tile_width}x{tile_height} px")
        print(f"BackColor: {backColor}")
        print(f"Tile count: {tileCount}")
        print(f"Tile columns: {tileColumns}")
        print(f"Level time: {levelTime}")

        # Decoding the data from Tiled format
        if encoding == 'csv':
            # CSV format map
            data_text = data_node.text.strip()
            tiles = [int(x) for x in data_text.replace('\n', '').split(',') if x]        
        elif encoding == 'base64':
            raw_data = base64.b64decode(data_node.text.strip())
            if compression == 'zlib':
                raw_data = zlib.decompress(raw_data)
            # Tiled base64 always uint32. Read and convert
            total_tiles = len(raw_data) // 4
            tiles = list(struct.unpack(f'<{total_tiles}I', raw_data))
        else:
            print(f"Error: Codificacion format '{encoding}' don't support by this script")
            return
        
        # Check if any tile ID > 255
        if any(t > 255 for t in tiles):
            print("¡WARNING!: some tiles ID are greater than 255 and will be truncated to uint8_t.")

        # Iterate each tile that has properties defined
        for tile in tileSet.findall('tile'):
            tile_id = int(tile.get('id')) + 1 # need to add 1 because the tileset starts with 0 but the map with 1
            properties = tile.find('properties')
            animation = tile.find('animation')

            if properties is not None:
                for prop in properties.findall('property'):
                    # Search the custom property called "property"
                    if prop.get('name') == 'property':
                        value = int(prop.get('value'))                        
                        tile_data.append((tile_id, value)) 

            if animation is not None:
                tile_frames = []
                for frame in animation.findall('frame'):
                    tile_frames.append({
                        'tileid': frame.get('tileid'),
                        'duration': frame.get('duration')
                    })
                tile_animations[tile_id] = tile_frames
   
        # Write binary file
        # Struct file:
        # [Header]
        #   - Tile Width                    (uint16)
        #   - Tile Height                   (uint16)
        #   - Map Width                     (uint16) 
        #   - Map Height                    (uint16)
        #   - Map Backcolor                 (uint16)
        #   - Level Time                    (uint16)
        #   - Tileset tile count            (uint16)
        #   - TIleset tile columns          (uint16)
        #   - Num tiles with data           (uint16)
        #   - Num tiles with animation      (uint16)
        # [Body]
        #   - Tile array            (uint8 * num_tiles)
        #   - Tile properties       (uint16* Num tiles with data)
        #   - Tile animations:
        #       -Tile id     (u8)
        #       -Num frames  (u8)
        #       -Frames:  
        #           -Frame tile id     (u8)
        #           -Frame duration    (u16)

        with open(bin_file, 'wb') as f:            
            # Pack the HEADER: 'H' = unsigned short (uint16_t, 2 bytes)
            header = struct.pack('<HHHHHHHHHH', tile_width, tile_height, map_width, map_height, backColor, levelTime, tileCount, tileColumns, len(tile_data), len(tile_animations))
            f.write(header)

            # Write tiles: 'B' = unsigned char (uint8_t, 1 byte)
            f.write(struct.pack(f'<{len(tiles)}B', *[t & 0xFF for t in tiles]))
            
            # Write tiles properties
            for tile_id, value in tile_data:
                f.write(struct.pack('<BH', tile_id, value))

            # Write tile animations
            for tile_id, frames in tile_animations.items():                
                # Write ID tile and how many frames
                print(f" Tile ID {tile_id} has {len(frames)} frames:")
                f.write(struct.pack('<BB', int(tile_id), len(frames)))
            
                # Write each frame (tile id and duration)
                for frame in frames:
                    print(f"  -  Frame TileID: {frame['tileid']} (Duration: {frame['duration']}ms)")
                    f.write(struct.pack('<BH', int(frame['tileid']), int(frame['duration'])))
            
            # Search entities layer            
            for obj_group in root.findall('objectgroup'):
                layerName = obj_group.get('name')
                if layerName != 'entities':
                    # If not found layer, jump to next
                    continue
                
                # Get object data
                # Write: numObjects (H) + list of (x0, y0, dir0)
                print(f" Processing layer: '{layerName}'")                
                
                # Extract entity data to temporal list
                objs = obj_group.findall('object')
                extracted_entities = []
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
                    spare = get_property_value(obj, "spare", default=0)

                    # store to dict for easy sort
                    extracted_entities.append({
                        'class_id': entClass,
                        'type_id': entType,
                        'x': ox,
                        'y': oy,
                        'dir': oDir,
                        'spare': spare,
                        'raw_class': raw_class, # for print info
                        'raw_type': raw_type # for print info
                    })

                # sort entity list by class_id
                extracted_entities.sort(key=lambda x: x['class_id'])

                # Write to bin file
                f.write(struct.pack('<H', len(extracted_entities))) # Write num objects                
                print(f" Processing {len(extracted_entities)} entities in custom order...")
                for ent in extracted_entities:
                    f.write(struct.pack('<BBHHBH', 
                        ent['class_id'], 
                        ent['type_id'], 
                        ent['x'], 
                        ent['y'], 
                        ent['dir'], 
                        ent['spare']
                    ))
                    print(f"\tWritten Class: {ent['raw_class']} - Type: {ent['raw_type']}")

            # Search enemies layer (deprecated. Enemies are on entity layer)
            """ foundEnemyLayer = 0            
            for obj_group in root.findall('objectgroup'):
                layerName = obj_group.get('name')
                if layerName != 'Enemies':
                    # Si la capa no está en nuestra lista, saltamos a la siguiente
                    continue
                
                # Get object data
                # Write: numObjects (H) + list of (x0, y0, dir0)
                print(f"✅ Processing layer: '{layerName}'")                
                
                objs = obj_group.findall('object')
                f.write(struct.pack('<H', len(objs))) # Write num objects
                print(f"📍 Processing objets: {len(objs)} objects")
                foundEnemyLayer = 1
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
                    spare = get_property_value(obj, "spare", default=0)

                    # write object data
                    f.write(struct.pack('<BBHHBH', entClass, entType, ox, oy, oDir, spare))                    
                    # print info
                    print(f"\tClass:{raw_class} - Type:{raw_type}")

            # if not found enemy layer, write 0 enemy count
            if not foundEnemyLayer:
                f.write(struct.pack('<H', 0)) """
        
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