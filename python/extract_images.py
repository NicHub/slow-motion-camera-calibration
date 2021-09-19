"""

Extract images from a video

pip install opencv-python

© september 2021, ouilogique.com

"""

import cv2
import os


def extract_images_from_video(video_fullpath, images_path, flip, px_locations):
    """
    flip =
       0 => flip verticaly
       1 => flip horizontaly
       other => don't flip
    """

    video_basename = os.path.basename(video_fullpath)
    video_name = os.path.splitext(video_basename)[0]
    capture_image = cv2.VideoCapture(video_fullpath)
    frame_count = 0

    while (True):

        con, frame = capture_image.read()

        if con:
            if flip in (0, 1):
                frame = cv2.flip(frame, flip)
            image_path = f"{images_path}{video_name}-flip{flip}-{frame_count:06}-nogit.jpg"
            print(image_path)
            # print(frame[719, 0])

            hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
            binary_num = 0
            for cnt, px_loc in enumerate(px_locations):
                val = hsv[px_loc[1], px_loc[0]]
                print(val)
                if val[2] > 250:
                    binary_num += 2 ** cnt
            print(f"binary_num = {binary_num}")
            print(f"binary_num = {bin(binary_num)}")


            cv2.imwrite(image_path, frame)
            frame_count += 1
            if frame_count > 1:
                break
        else:
            break


if __name__ == "__main__":

    images_path = os.path.expanduser(
        "~/Documents/PlatformIO/Projects/camera-slow-motion-calibration/images-nogit/")
    if not os.path.exists(images_path):
        os.makedirs(images_path)
    video_fullpath = os.path.expanduser(
        "~/Documents/PlatformIO/Projects/camera-slow-motion-calibration/movies-nogit/20210919_145432.mp4")

    px_locations = (
        (908, 486),
        (860, 486),
        (809, 512),
        (754, 496),
        (700, 487),
        (657, 486),
        (604, 483),
        (551, 486),
        (508, 487),
        (455, 500),
        (405, 499),
        (345, 494),
        (294, 494),
        (248, 494),
    )
    extract_images_from_video(video_fullpath, images_path, 1, px_locations)
