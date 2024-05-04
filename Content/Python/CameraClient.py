# Copyright Tempo Simulation, LLC. All Rights Reserved.

import asyncio
import grpc
from TempoScripting import Empty_pb2
from Tempo import Camera_pb2
from Tempo import Camera_pb2_grpc

async def main():
    async with grpc.aio.insecure_channel("localhost:10002") as channel:
        stub = Camera_pb2_grpc.CameraServiceStub(channel)
        message = Empty_pb2.Empty()
        async for image in stub.StreamImages(message):
            print(image)

if __name__ == "__main__":
    asyncio.run(main())
