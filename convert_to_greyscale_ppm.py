from PIL import Image
img = Image.open("engine.png").convert("L").resize((512,512))
img.save("sobel_in_greyscale.ppm", format="PPM")