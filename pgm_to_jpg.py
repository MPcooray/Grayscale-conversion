# pgm_to_jpg.py
# Converts a .pgm grayscale image to .jpg using Pillow (PIL)
# Usage:
#   python pgm_to_jpg.py myphoto_gray.pgm

from PIL import Image
import sys
import os

# ---- Step 1: read command-line argument ----
if len(sys.argv) < 2:
    print("Usage: python pgm_to_jpg.py <input.pgm>")
    sys.exit(1)

input_file = sys.argv[1]

if not os.path.exists(input_file):
    print(f"Error: file '{input_file}' not found.")
    sys.exit(1)

# ---- Step 2: open and convert ----
try:
    img = Image.open(input_file)
    img = img.convert("RGB")  # ensure it's RGB for JPEG output
except Exception as e:
    print(f"Error opening {input_file}: {e}")
    sys.exit(1)

# ---- Step 3: build output name ----
base = os.path.splitext(input_file)[0]
output_file = base + ".jpg"

# ---- Step 4: save ----
try:
    img.save(output_file, format="JPEG", quality=95)
    print(f"Saved '{output_file}' successfully.")
except Exception as e:
    print(f"Error saving {output_file}: {e}")
    sys.exit(1)
