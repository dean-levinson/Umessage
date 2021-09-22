import logging
from asyncio.streams import StreamReader, StreamWriter

from struct_wrapper import Field, StructWrapper
from server_consts import *
from server_utils import null_terminated_bytes_to_str

RESPONSES = {}

def response_code(code):
    def deco(inner):
        RESPONSES[code] = inner
        return inner
    return deco

class Response(object):
    def __init__(self, reader: StreamReader, writer: StreamWriter) -> None:
        self.reader = reader
        self.writer = writer

    async def fetch_request(self):
        raise NotImplementedError("Abstract class")

    async def respond(self):
        raise NotImplementedError("Abstract class")

@response_code(1000)
class RegisterResponse(Response):
    async def fetch_request(self, request_size):
        received_bytes = await self.reader.read(request_size)
        self.client_name, self.public_key = StructWrapper(Fields.CLIENT_NAME,
                                                          Fields.PUBLIC_KEY).unpack(received_bytes)

        self.client_name = null_terminated_bytes_to_str(self.client_name)
        logging.info(f"Client_name - {self.client_name}, Public key - {self.public_key}")
