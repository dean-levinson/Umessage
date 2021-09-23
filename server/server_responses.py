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
    async def fetch_and_respond(self, request_size):
        received_bytes = await self._reader.read(request_size)
        client_name, public_key = StructWrapper(Fields.CLIENT_NAME,
                                                Fields.PUBLIC_KEY).unpack(received_bytes)

        client_name = null_terminated_bytes_to_str(client_name)

        try:
            user = self._server.users.add_user(client_name, public_key)
            logging.info(f"New user is added - {user}")
            payload = StructWrapper(Fields.CLIENT_ID).pack(user.client_id)
            await self.respond(payload)

        except UserAlreadyExists:
            logging.warning(f"Got register request for an existing client name - '{client_name}'")
            await self.respond_error()


@response_code(1001)
class ClientListResponse(Response):
    async def fetch_and_respond(self, request_size):
        if not (request_size == 0):
            logging.error("Payload size of client list request should be 0")
            await self.respond_error()
            return

        payload = bytes()
        for user in self._server.users:
            payload += StructWrapper(Fields.CLIENT_ID, Fields.CLIENT_NAME).pack(user.client_id, user.client_name)

        await self.respond(payload)


@response_code(1002)
class PublicKeyResponse(Response):
    async def fetch_and_respond(self, request_size):
        received_bytes = await self._reader.read(request_size)
        client_id = StructWrapper(Fields.CLIENT_ID).unpack(received_bytes)[0]

        user = self._server.users.get_user_by_client_id(client_id)

        if not user:
            await self.respond_error()
            return
        
        # very bad code
        public_key = user.pubkey

        if not user.pubkey:
            # Todo - realise what I got to do
            public_key = b"1" * 160

        payload = StructWrapper(Fields.CLIENT_ID, Fields.PUBLIC_KEY).pack(client_id, public_key)
        await self.respond(payload)
