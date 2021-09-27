import asyncio
import logging

from umessage_server import UMessageServer


logging.basicConfig(level=logging.DEBUG, format="[%(levelname)s]: %(message)s")


async def main():
    app = UMessageServer()
    server_loop = await asyncio.start_server(
        app.handle_client, "127.0.0.1", 5442)
    
    print("Starting server on:", server_loop.sockets[0].getsockname())

    async with server_loop:
        await server_loop.serve_forever()

if __name__ == "__main__":
    asyncio.run(main())
