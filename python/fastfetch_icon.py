from PIL import Image
import sys

CHARS = (
    "@#8&B%$WM*oahkbdpwmZO0QLCJUYXzcv"
    "unxrjft/|()1{}[]?-_+~<>i!lI^   "
)

WIDTH = 20
HEIGHT = 10

if len(sys.argv) != 2:
    print(f"Usage: {sys.argv[0]} image.png")
    sys.exit(1)

image = Image.open(sys.argv[1]).convert("L")

image = image.resize((WIDTH, HEIGHT), Image.Resampling.LANCZOS)

print("char fastfetch_icon[20][11] = {")

for y in range(HEIGHT):
    line = ""

    for x in range(WIDTH):
        pixel = image.getpixel((x, y))
        pixel = 255 - pixel

        index = pixel * (len(CHARS) - 1) // 255
        line += CHARS[index]

    print(f'    "{line}",')

print("};")