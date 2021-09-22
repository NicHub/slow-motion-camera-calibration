"""

Extract images from a video

pip install opencv-python

© september 2021, ouilogique.com

"""

import cv2
import os


def mean_hsv_val_around_px(hsv, px_loc):
    """ ___ """
    mean_h = 0
    mean_s = 0
    mean_v = 0
    roi_half_size = 5
    pixel_count = roi_half_size*roi_half_size * 4
    counter = 0
    for _x in range(px_loc[1]-roi_half_size, px_loc[1]+roi_half_size):
        for _y in range(px_loc[0]-roi_half_size, px_loc[0]+roi_half_size):
            counter += 1
            mean_h += hsv[_x, _y][0]
            mean_s += hsv[_x, _y][1]
            mean_v += hsv[_x, _y][2]

    mean_v = int(round(mean_v / pixel_count, 0))
    return mean_v


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

    binary_nums = []
    while True:

        con, frame = capture_image.read()

        if con:
            frame = cv2.rotate(frame, cv2.cv2.ROTATE_90_CLOCKWISE)
            if flip in (0, 1):
                frame = cv2.flip(frame, flip)
            image_path = f"{images_path}{video_name}-flip{flip}-{frame_count:06}-nogit.jpg"
            hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
            binary_num = 0
            for cnt, px_loc in enumerate(px_locations):
                mean_v = mean_hsv_val_around_px(hsv, px_loc)
                bit_val = mean_v > 253

                val = hsv[px_loc[1], px_loc[0]]
                str = "0"
                color = (0, 255, 0, 255)
                if bit_val:
                    str = "1"
                    color = (0, 0, 255, 255)
                    binary_num += 2 ** cnt

                pos = (px_loc[0], 540)
                cv2.circle(frame, px_loc, 25, color, 5)
                cv2.circle(frame, pos, 20, (0, 0, 0), -1)
                cv2.putText(
                    frame,
                    f"{mean_v}",
                    (pos[0]-10, pos[1]+10),
                    cv2.FONT_HERSHEY_SIMPLEX,  # font family
                    0.5,  # font size
                    color,
                    2)  # font stroke

            # print(f"binary_num = {binary_num} = {bin(binary_num)}")
            # print(f"{binary_num}")
            binary_nums.append(f"{binary_num}")

            cv2.imwrite(image_path, frame)
            frame_count += 1
            # if frame_count > 20:
            #     break
        else:
            break

    ans = "\n".join(binary_nums)
    _f = open("camera-calib.csv", "w")
    _f.write(ans)
    _f.close()



if __name__ == "__main__":

    source = 3

    if source == 0:
        images_path = os.path.expanduser(
            "~/Documents/PlatformIO/Projects/camera-slow-motion-calibration/images-nogit/")
        if not os.path.exists(images_path):
            os.makedirs(images_path)
        video_fullpath = os.path.expanduser(
            "~/Documents/PlatformIO/Projects/camera-slow-motion-calibration/movies-nogit/20210919_145432.mp4")
        flip = 1
        px_locations = (
            (908, 493),
            (856, 485),
            (805, 508),
            (754, 498),
            (704, 486),
            (654, 485),
            (604, 485),
            (555, 486),
            (508, 487),
            (455, 500),
            (405, 499),
            (345, 494),
            (294, 494),
            (248, 494),
        )

    if source == 1:
        images_path = os.path.expanduser(
            "~/Documents/PlatformIO/Projects/camera-slow-motion-calibration/images-nogit/")
        if not os.path.exists(images_path):
            os.makedirs(images_path)
        video_fullpath = os.path.expanduser(
            "~/Documents/PlatformIO/Projects/camera-slow-motion-calibration/movies-nogit/20210920_171411.mp4")
        flip = -1
        px_locations = (
            (970, 367),
            (916, 365),
            (864, 356),
            (807, 369),
            (753, 365),
            (704, 348),
            (651, 343),
            (598, 344),
            (546, 340),
            (496, 336),
            (443, 344),
            (388, 359),
            (337, 346),
            (291, 349),
        )

    if source == 2:
        images_path = os.path.expanduser(
            "~/Desktop/camera-slow-motion-calibration/images-nogit/")
        if not os.path.exists(images_path):
            os.makedirs(images_path)
        video_fullpath = os.path.expanduser(
            "~//Desktop/camera-slow-motion-calibration/videos-nogit/20210922_123911.mp4")
        flip = -1
        px_locations = (
            (1167, 380),
            (1086, 381),
            (1009, 369),
            (929, 397),
            (846, 397),
            (766, 375),
            (683, 381),
            (610, 381),
            (530, 377),
            (453, 378),
            (365, 399),
            (281, 418),
            (194, 377),
            (113, 406),
        )

    if source == 3:
        images_path = os.path.expanduser(
            "~/Desktop/camera-slow-motion-calibration/images-nogit/")
        if not os.path.exists(images_path):
            os.makedirs(images_path)
        video_fullpath = os.path.expanduser(
            "~//Desktop/camera-slow-motion-calibration/videos-nogit/20210922_133004.mp4")
        flip = -1
        px_locations = (
            (242, 469),
            (306, 458),
            (366, 469),
            (420, 460),
            (478, 460),
            (544, 469),
            (598, 480),
            (652, 477),
            (714, 483),
            (770, 481),
            (832, 469),
            (890, 459),
            (951, 485),
            (1013, 465),
        )

    extract_images_from_video(video_fullpath, images_path, flip, px_locations)
