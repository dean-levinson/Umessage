import asyncio
import logging
from typing import Type

from users import Users
from messages import Messages
import server_requests
import server_responses


class UMessageServer(object):    
    def __init__(self) -> None:
        self.users = Users()
        self.messages = Messages()

    async def handle_client(self, reader, writer):
        logging.info(f"Got connection from {writer.get_extra_info('sockname')}")
        while True:
            request_headers = server_requests.RequestHeaders(reader)
            await request_headers.fetch()
            response_cls = server_responses.RESPONSES[request_headers.code] # type: Type[server_responses.Response]
            response = response_cls(self, reader, writer)
            await response.fetch_and_respond(request_headers.payload_size)
