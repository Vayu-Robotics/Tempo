import tempo.tempo_sensors as ts
import numpy as np
import cv2
import asyncio
import time
import io
import argparse


def show_depth_image(image, name):
    image_array = np.asarray(image.depths)
    image_array = image_array.reshape(image.height, image.width)
    image_array.astype(np.float32)
    cv2.imshow(f"Camera {name}", image_array)
    cv2.waitKey(1)


def show_color_image(image, name):
    image_file = io.BytesIO(image.data)
    img = cv2.imdecode(np.frombuffer(image_file.getvalue(), np.uint8), cv2.IMREAD_COLOR)
    cv2.imshow(f"Camera {name}", img)
    cv2.waitKey(1)


def show_label_image(image, name):
    image_bytes = io.BytesIO(image.data)
    image_array = np.frombuffer(image_bytes.getvalue(), dtype=np.uint8)
    image_array = image_array.reshape((image.height, image.width))
    # Normalize the byte values for better hue representation (0-179)
    normalized_data = np.uint8(image_array)
    bgr_image = cv2.cvtColor(normalized_data, cv2.COLOR_GRAY2BGR)
    hsv_image = cv2.cvtColor(bgr_image, cv2.COLOR_BGR2HSV)
    # saturation = np.full((image.height, image.width), 255, dtype=np.uint8)
    # value = np.full((image.height, image.width), 255, dtype=np.uint8)
    # hsv_full = cv2.merge([hsv_image])
    bgr_image = cv2.cvtColor(hsv_image, cv2.COLOR_HSV2BGR)
    cv2.imshow(f"Camera {name}", hsv_image)
    cv2.waitKey(1)


async def stream_color_images(camera_id):
    async for image in ts.stream_color_images(camera_id):
        show_color_image(image, camera_id)


async def stream_depth_images(camera_id):
    async for image in ts.stream_depth_images(camera_id):
        show_depth_image(image, camera_id)


async def stream_label_images(camera_id):
    async for image in ts.stream_label_images(camera_id):
        show_label_image(image, camera_id)


async def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--type', required=True)
    parser.add_argument('--names', nargs='*', required=True)
    args = parser.parse_args()
    available_sensors_response = await ts.get_available_sensors()
    ids_to_stream = []
    for available_sensor in available_sensors_response.available_sensors:
        if available_sensor.name in args.names:
            ids_to_stream.append(available_sensor.id)
    stream_func = None
    if args.type == "color":
        stream_func = stream_color_images
    elif args.type == "depth":
        stream_func = stream_depth_images
    elif args.type == "label":
        stream_func = stream_label_images
    await asyncio.gather(*[stream_func(id) for id in ids_to_stream])

if __name__ == "__main__":
    asyncio.run(main())
