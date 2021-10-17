import asyncio
import logging

from umessage_server import UMessageServer
from server_consts import PORT_INFO_PATH


logging.basicConfig(level=logging.DEBUG, format="[%(levelname)s]: %(message)s")

def get_port_from_file():
    with open(PORT_INFO_PATH, "rb") as fp:
        return int(fp.read())

async def main():
    app = UMessageServer()
    server_loop = await asyncio.start_server(
        app.handle_client, "127.0.0.1", get_port_from_file())
    
    print("Starting server on:", server_loop.sockets[0].getsockname())

    async with server_loop:
        await server_loop.serve_forever()

if __name__ == "__main__":
    asyncio.run(main())
