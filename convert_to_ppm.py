from PIL import Image
import sys, os

if len(sys.argv) < 2:
    print("Usage: python convert_to_ppm.py photo.jpg")
    sys.exit(1)

src = sys.argv[1]
im = Image.open(src).convert("RGB")
out = os.path.splitext(src)[0] + ".ppm"
im.save(out, format="PPM")
print(f"Saved {out}")
