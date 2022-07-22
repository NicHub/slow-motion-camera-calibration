"""

pip install -r requirements.txt

© september 2021, ouilogique.com

"""

import os

import cv2
import numpy as np
from bokeh.plotting import figure, output_file, show
import yaml


def create_output_dir(camera_info):
    """ ___ """

    if not os.path.exists(camera_info["frames_directory_path"]):
        os.makedirs(camera_info["frames_directory_path"])


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


def extract_frames_from_video_and_evaluate_binary_number_displayed(camera_info):
    """ ___ """

    _video_basename = os.path.basename(camera_info["video_file_path"])
    _video_name = os.path.splitext(_video_basename)[0]
    _capture_image = cv2.VideoCapture(camera_info["video_file_path"])
    _total_frame_count = int(_capture_image.get(cv2.CAP_PROP_FRAME_COUNT))
    # _total_frame_count = 10
    binary_clock_values = []

    print(
        f'\n\rExtracting frames of:        \"{camera_info["video_file_path"]}\"\n', end="")
    print(f"\rNumber of frames to extract: {_total_frame_count}\n", end="")

    for _frame_count in range(_total_frame_count):

        # Extract frame from video.
        # Rotate it.
        # Flip it.
        _, _frame = _capture_image.read()
        print(f'\rExtracting frame {_frame_count}', end=" ")
        if camera_info["rotate"] in (0, 1, 2):
            _frame = cv2.rotate(_frame, camera_info["rotate"])
        if camera_info["flip"] in (0, 1):
            _frame = cv2.flip(_frame, camera_info["flip"])

        # Evaluate if the LEDs are ON or OFF
        # and calculate the binary number value displayed.
        _hsv = cv2.cvtColor(_frame, cv2.COLOR_BGR2HSV)
        _binary_clock_value = 0
        for _cnt, _px_loc in enumerate(camera_info["led_coord_px"]):
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
        binary_clock_values.append(_binary_clock_value)

        # Save the frame.
        _image_path = f'{camera_info["frames_directory_path"]}{_video_name}-{_frame_count:06}.jpg'
        cv2.imwrite(_image_path, _frame)

    # Save the binary number values for each frame in a CSV file.
    binary_clock_values_string = "\n".join(str(n) for n in binary_clock_values)
    with open(camera_info["result_file_name"], "w") as _f:
        _f.write(binary_clock_values_string)
    print(
        f'\rAnalysis saved under:        \"{camera_info["result_file_name"]}\"\n', end="")
    print("\r", end="")

    # Store results.
    camera_info["binary_clock_values"] = arr = np.array(binary_clock_values)


def get_camera_info():
    """ ___ """

    with open("camera_data.yaml", "rt") as _f:
        data = yaml.full_load(_f.read())

    for _d in data:
        _d["rotate"] = eval(_d["rotate"])
        _d["led_coord_px"] = eval(_d["led_coord_px"])

    return data


def analyze_results(camera_info):
    """ ___ """

    binary_clock_values_diff2 = np.diff(camera_info["binary_clock_values"], 2)
    _filter_criterion = 15
    _filter = np.absolute(binary_clock_values_diff2) > _filter_criterion
    _indexes_to_filter_out = np.where(_filter)[0] + 1
    camera_info["binary_clock_values"][_indexes_to_filter_out] = 0
    print(f"filter criterion:            {_filter_criterion}")
    print(f"bad measurements count:      {len(_indexes_to_filter_out)}")
    print(
        f'total measurements count:    {len(camera_info["binary_clock_values"])}')

    output_file(camera_info["html_output_file_name"])
    p = figure(width=2000, height=1000)
    p.line(range(len(camera_info["binary_clock_values"])),
           camera_info["binary_clock_values"],
           line_width=2, color="red", alpha=0.5)
    p.circle(range(len(camera_info["binary_clock_values"])),
             camera_info["binary_clock_values"],
             size=4, color="red", alpha=0.5)
    p.line(range(len(binary_clock_values_diff2)),
           binary_clock_values_diff2,
           line_width=2, color="navy", alpha=0.5)
    p.circle(range(len(binary_clock_values_diff2)),
             binary_clock_values_diff2,
             size=4, color="navy", alpha=0.5)
    p.line(range(len(_filter)),
           _filter,
           line_width=2, color="navy", alpha=0.5)
    p.circle(_indexes_to_filter_out,
             camera_info["binary_clock_values"][_indexes_to_filter_out],
             size=20, color="green", alpha=0.5)
    p.line(_indexes_to_filter_out,
           camera_info["binary_clock_values"][_indexes_to_filter_out],
           line_width=2, color="green", alpha=0.5)
    show(p)


def read_csv(camera_info):
    """ ___ """

    camera_info["binary_clock_values"] = np.loadtxt(
        camera_info["result_file_name"], dtype=int)


if __name__ == "__main__":

    camera_info_list = get_camera_info()
    camera_info_list = [camera_info_list[1]]

    for camera_info in camera_info_list:
        if not os.path.exists(camera_info["video_file_path"]):
            print(f'File not found: {camera_info["video_file_path"]}')
            continue
        create_output_dir(camera_info)
        debug = False
        if debug:
            read_csv(camera_info)
        else:
            extract_frames_from_video_and_evaluate_binary_number_displayed(
                camera_info)
        analyze_results(camera_info)
