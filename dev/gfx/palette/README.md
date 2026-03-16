# Game palette

The first color is transparent color
The first 64 colors is for persistent images: player, hud, items colors
The next 192 colors is for each level/enemies/level objects

# Method for merge palettes

- Open RGB level design
- Create palette from image with 191 limit colors (190 + 1 for the transparent color)
- Convert to indexed
- Palette size to 256
- Cut and paste the actual colors starting on position 65 and remap
- Open playerHudItems palette file and copy the 64 colors
- Paste on palette position 0 and remap
