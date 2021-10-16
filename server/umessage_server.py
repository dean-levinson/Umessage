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
        try: 
            peer = writer.get_extra_info('peername')
            logging.info(f"Client connected from {peer}")
            while True:
                request_headers = server_requests.RequestHeaders(reader)
                await request_headers.fetch()
                logging.debug(f"Got request {request_headers}")
                try:
                    response_cls = server_responses.RESPONSES[request_headers.code] # type: Type[server_responses.Response]
                except KeyError: # No such code
                    logging.error("Got unknown request code from client. ignoring...")
                    continue
                response = response_cls(self, reader, writer)
                await response.fetch_and_respond(request_headers)

        except (asyncio.streams.IncompleteReadError, ConnectionResetError):
            logging.info(f"Client {peer} as disconnected")