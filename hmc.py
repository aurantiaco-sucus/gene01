# Heightmap compilation script

from PIL import Image

c1 = (158, 217, 246)
c2 = (203, 226, 163)
c3 = (255, 250, 188)
c4 = (251, 203, 114)
c5 = (222, 163, 83)

img = Image.open('heightmap.png').convert('RGB')

with open('heightmap.txt', 'w') as o:
    for y in range(img.height):
        for x in range(img.width):
            if img.getpixel((x, y)) == c1:
                o.write('0')
            elif img.getpixel((x, y)) == c2:
                o.write('1')
            elif img.getpixel((x, y)) == c3:
                o.write('2')
            elif img.getpixel((x, y)) == c4:
                o.write('3')
            elif img.getpixel((x, y)) == c5:
                o.write('4')
            else:
                raise ValueError('invalid color')