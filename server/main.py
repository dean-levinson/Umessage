import asyncio
import logging

from handler import handle_client

logging.basicConfig(level=logging.INFO, format="[%(levelname)s]: %(message)s")


async def main():
    server = await asyncio.start_server(
        handle_client, "127.0.0.1", 5442)
    
    print("Starting server on:", server.sockets[0].getsockname())

    async with server:
        await server.serve_forever()

if __name__ == "__main__":
    asyncio.run(main())
