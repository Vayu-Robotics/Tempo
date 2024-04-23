# Copyright Tempo Simulation, LLC. All Rights Reserved.

import asyncio
import grpc
import Time_pb2
import Time_pb2_grpc

async def main():
    import grpc
    import Time_pb2
    import Time_pb2_grpc
    async with grpc.aio.insecure_channel("localhost:10002") as channel:
        stub = Time_pb2_grpc.TimeStub(channel)
        message = Time_pb2.Empty()
        while True:
            command = input("Command: ")
            if command in {"Pause", "pause"}:
                response = await stub.Pause(message)
            elif command in {"Play", "play"}:
                response = await stub.Play(message)

if __name__ == "__main__":
    asyncio.run(main())
