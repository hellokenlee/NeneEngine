# -*- coding:UTF-8 -*-
# __author__ = "KenLee"
# __email__ = "hellokenlee@163.com"

import time
from PIL import Image

NEIGHBORS = [
    (-1, -1), (0, -1), (1, -1),
    ( 0, -1),          (1,  0),
    (-1,  1), (0,  1), (1,  1),
]


def all_equal_to(atuple, val):
    for v in atuple:
        if v != val:
            return False
    return True


def mix_colors(colors, channels=4):
    result = [0] * channels
    for color in colors:
        for c in range(channels):
            result[c] = result[c] + color[c]
    for c in range(channels):
        result[c] = int(result[c] / len(colors))
    return tuple(result)


def main(src_path, mask_path):
    # type: (str, str) -> None
    #
    src_image = Image.open(src_path, mode='r')
    mask_image = Image.open(mask_path, mode='r')
    #
    assert(src_image.size[0] == mask_image.size[0])
    assert(src_image.size[1] == mask_image.size[1])
    #
    assert(src_image.mode == "RGBA")
    src_image_data = src_image.load()
    mask_image_data = mask_image.load()
    #
    for y in range(src_image.size[1]):
        for x in range(src_image.size[0]):
            if all_equal_to(mask_image_data[x, y], 0):
                #
                neighbor_colors = []
                #
                for dx, dy in NEIGHBORS:
                    nx = x + dx
                    ny = y + dy
                    if -1 < nx < src_image.size[0] and -1 < ny < src_image.size[1]:
                        if not all_equal_to(mask_image_data[nx, ny], 0):
                            neighbor_colors.append(src_image_data[nx, ny])
                #
                if len(neighbor_colors) > 0:
                    src_image_data[x, y] = mix_colors(neighbor_colors)
        print("%.2f%%" % (float(y) / float(src_image.size[1]) * 100.0))
    #
    src_image.save("LappedCoordPadded.png")
    pass


if __name__ == '__main__':
    start = time.time()
    src_path = "./LappedCoord.png"
    mask_path = "./FlattenCoord.png"
    main(src_path, mask_path)
    end = time.time()
    print("Finished with: %f" % (end - start))
    pass