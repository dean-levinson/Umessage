import logging
from asyncio.streams import StreamReader, StreamWriter
from server_exceptions import UserAlreadyExists

from struct_wrapper import Field, StructWrapper
from server_consts import *
from server_utils import null_terminated_bytes_to_str

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from umessage_server import UMessageServer

RESPONSES = {}

def response_code(code):
    def deco(inner):
        RESPONSES[code] = inner
        inner.CODE = code
        return inner
    return deco

class Response(object):
    def __init__(self, server, reader: StreamReader, writer: StreamWriter) -> None:
        self._server = server # type: UMessageServer
        self._reader = reader
        self._writer = writer

    async def fetch_request(self):
        raise NotImplementedError("Abstract class")

    async def respond(self):
        raise NotImplementedError("Abstract class")
    
    async def write(self, payload):
        self._writer.write(payload)
        await self._writer.drain()

    async def respond_headers(self, payload_size, code=None):
        if not code:
            code = self.CODE
        await self.write(StructWrapper(Fields.VERSION,
                                       Fields.CODE,
                                       Fields.PAYLOAD_SIZE).pack(SERVER_VERSION,
                                                                 code,
                                                                 payload_size))

    async def respond(self, payload):
        await self.respond_headers(len(payload))
        await self.write(payload)
    
    async def respond_error(self):
        await self.respond_headers(0, code=SERVER_ERROR_CODE)

@response_code(1000)
class RegisterResponse(Response):
    def __init__(self, server, reader: StreamReader, writer: StreamWriter) -> None:
        super().__init__(server, reader, writer)
        self.client_name = None
        self.public_key = None

    async def fetch_and_respond(self, request_size):
        received_bytes = await self._reader.read(request_size)
        self.client_name, self.public_key = StructWrapper(Fields.CLIENT_NAME,
                                                          Fields.PUBLIC_KEY).unpack(received_bytes)

        self.client_name = null_terminated_bytes_to_str(self.client_name)

        try:
            logging.info(f"Adding new user - {self.client_name}")
            user = self._server.users.add_user(self.client_name, self.public_key)
            payload = StructWrapper(Fields.CLIENT_ID).pack(user.client_id)
            await self.respond(payload)

        except UserAlreadyExists:
            logging.warning("Got register request for an existing client name")
            await self.respond_error()
