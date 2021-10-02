"""

Extract frames from a video

pip install opencv-python

© september 2021, ouilogique.com

"""

import cv2
import os


def create_output_dir(settings):
    """ ___ """

    if not os.path.exists(settings["frames_directory_path"]):
        os.makedirs(settings["frames_directory_path"])


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


def extract_and_analyze_frames_from_video(settings):
    """ ___ """

    _video_basename = os.path.basename(settings["video_file_path"])
    _video_name = os.path.splitext(_video_basename)[0]
    _capture_image = cv2.VideoCapture(settings["video_file_path"])
    _total_frame_count = int(_capture_image.get(cv2.CAP_PROP_FRAME_COUNT))
    _binary_clock_values = []

    print(
        f'\n\rExtracting frames of:        \"{settings["video_file_path"]}\"\n', end="")
    print(f"\rNumber of frames to extract: {_total_frame_count}\n", end="")

    for _frame_count in range(_total_frame_count):

        # Extract frame from video.
        # Rotate it.
        # Flip it.
        _, _frame = _capture_image.read()
        print(f'\rExtracting frame {_frame_count}', end=" ")
        if settings["rotate"] in (0, 1, 2):
            _frame = cv2.rotate(_frame, settings["rotate"])
        if settings["flip"] in (0, 1):
            _frame = cv2.flip(_frame, settings["flip"])

        # Evaluate if the LEDs are ON or OFF
        # and calculate the binary number value displayed.
        _hsv = cv2.cvtColor(_frame, cv2.COLOR_BGR2HSV)
        _binary_clock_value = 0
        for _cnt, _px_loc in enumerate(settings["led_coord_px"]):
            _mean_v = mean_hsv_val_around_px(_hsv, _px_loc)
            _bit_val = _mean_v > 253

            _color = (0, 255, 0, 255)
            if _bit_val:
                _color = (0, 0, 255, 255)
                _binary_clock_value += 2 ** _cnt

            _pos = (_px_loc[0], 540)
            cv2.circle(_frame, _px_loc, 25, _color, 5)
            cv2.circle(_frame, _pos, 20, (0, 0, 0), -1)
            cv2.putText(
                _frame,
                f"{_mean_v}",
                (_pos[0]-15, _pos[1]+5),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.5,  # font size
                _color,
                2)  # font stroke
        _binary_clock_values.append(f"{_binary_clock_value}")

        # Save the frame.
        _image_path = f'{settings["frames_directory_path"]}{_video_name}-{_frame_count:06}.jpg'
        cv2.imwrite(_image_path, _frame)

    # Save the binary number values for each frame in a CSV file.
    _binary_clock_values_string = "\n".join(_binary_clock_values)
    _result_file_name = f'binary-clock-values-{_video_name}.csv'
    with open(_result_file_name, "w") as _f:
        _f.write(_binary_clock_values_string)
    print(f"\rAnalysis saved under:         \"{_result_file_name}\"\n", end="")
    print("\r", end="")


def get_settings():
    """
    # SETTINGS CONTENT

    "video_file_path" : Path to the video file. It must exist before running this program.
    "frames_directory_path": Path to the directory where frames will be saved. Will be created if needed.
    "rotate":
        0 => cv2.cv2.ROTATE_90_CLOCKWISE
        1 => cv2.cv2.ROTATE_180
        2 => cv2.cv2.ROTATE_90_COUNTERCLOCKWISE
        other => don’t rotate
    "flip":
        0 => flip verticaly
        1 => flip horizontaly
        other => don’t flip
    "led_coord_px": X-Y coordinates in pixel of the LED in the frame. Can be found with "get-px-coordinates.html".
    """

    settings_list = [
        {
            "video_file_path": os.path.expanduser(
                "../videos/20210920_171411.mp4"),
            "frames_directory_path": os.path.expanduser(
                "../frames-20210920_171411-nogit/"),
            "rotate": cv2.cv2.ROTATE_90_CLOCKWISE,
            "flip": -1,
            "led_coord_px": (
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
        },
        {
            "video_file_path": os.path.expanduser(
                "../videos/20210922_123911.mp4"),
            "frames_directory_path": os.path.expanduser(
                "../frames-20210922_123911-nogit/"),
            "rotate": cv2.cv2.ROTATE_90_CLOCKWISE,
            "flip": -1,
            "led_coord_px": (
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
        },
        {
            "video_file_path": os.path.expanduser(
                "../videos/20210922_133004.mp4"),
            "frames_directory_path": os.path.expanduser(
                "../frames-20210922_133004-nogit/"),
            "rotate": cv2.cv2.ROTATE_90_COUNTERCLOCKWISE,
            "flip": -1,
            "led_coord_px": (
                (1038, 251),
                (974, 262),
                (914, 251),
                (860, 260),
                (802, 260),
                (736, 251),
                (682, 240),
                (628, 243),
                (566, 237),
                (510, 239),
                (448, 251),
                (390, 261),
                (329, 235),
                (267, 255),

            ),
        }
    ]
    return settings_list


if __name__ == "__main__":

    settings_list = get_settings()
    settings_list = [settings_list[2]]

    for settings in settings_list:
        if not os.path.exists(settings["video_file_path"]):
            print(f'File not found: {settings["video_file_path"]}')
            continue
        create_output_dir(settings)
        extract_and_analyze_frames_from_video(settings)
