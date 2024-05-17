import tempo.tempo_sensors as ts
import numpy as np
import cv2
import asyncio
import time
import io


def show_image(image, name):
    # width, height = (image.width, image.height)
    # packed_data = np.array(image.pixels, dtype=np.uint32)
    # image_data = packed_data.view(dtype=np.uint8).reshape((height, width, 4))  # Assuming ARGB
    # cv2.imshow(f"Camera {name}", image_data)
    # cv2.waitKey(1)
    image_file = io.BytesIO(image.data)
    img = cv2.imdecode(np.frombuffer(image_file.getvalue(), np.uint8), cv2.IMREAD_COLOR)
    cv2.imshow(f"Camera {name}", img)
    cv2.waitKey(1)


def stream_images_sync():
    for image in ts.stream_images(0):
        show_image(image)


async def stream_images_async(camera_id):
    overall_count = 0
    count = 0
    async for image in ts.stream_images(camera_id, compression_level=0):
        # print(len(image.data))
        overall_count += 1
        if overall_count == 20:
            start = time.time()
        if overall_count > 20:
            count += 1
            end = time.time()
            diff = end - start
            print("Got {} images in {} seconds from camera {} ({} per second)".format(count, diff, camera_id, count / diff))
        # show_image(image, camera_id)


async def main():
    await asyncio.gather(stream_images_async(0), stream_images_async(1), stream_images_async(2), stream_images_async(3),
                         stream_images_async(4), stream_images_async(5), stream_images_async(6), stream_images_async(7),
                         stream_images_async(8), stream_images_async(9), stream_images_async(10), stream_images_async(11),
                         stream_images_async(12), stream_images_async(13), stream_images_async(14), stream_images_async(15))


if __name__ == "__main__":
    asyncio.run(main())
    # stream_images_sync()
