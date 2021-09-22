import asyncio
import logging
from typing import Type

import server_requests
import server_responses


async def handle_client(reader, writer):
    logging.info(f"Got connection from {writer.get_extra_info('sockname')}")
    while True:
        request_headers = server_requests.RequestHeaders(reader)
        await request_headers.fetch()
        response_cls = server_responses.RESPONSES[request_headers.code] # type: Type[server_responses.Response]
        response = response_cls(reader, writer)
        await response.fetch_request(request_headers.payload_size)
        # await response.respond()
