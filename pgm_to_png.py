from PIL import Image
im= Image.open("output.pgm")
im.save("output.png")
print("Converted output.pgm to output.png.")


# # ppm_to_png.py
# from PIL import Image
# im = Image.open("input.ppm")
# im.save("input.png")
# print("Saved input.png")
