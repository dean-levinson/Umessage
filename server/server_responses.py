import logging
from asyncio.streams import StreamReader, StreamWriter
from mmap import ACCESS_READ
from server_exceptions import UserAlreadyExists

from struct_wrapper import Field, StructWrapper
from server_consts import *
from server_utils import null_terminated_bytes_to_str
from server_requests import RequestHeaders

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from umessage_server import UMessageServer

RESPONSES = {}

def response_code(code):
    """
    Class Decorator that wraps Response class. Maps between response codes and the corresponding 
    classes that should handle them. Uses global RESPONSES dict to store the mapping.
    """
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

    async def fetch_and_respond(self, request_headers: RequestHeaders):
        """
        Fetches the rest of the client's request using the given request's headers data.
        Respond to the client's request with corresponding response.
        """        
        raise NotImplementedError("Abstract class")
    
    async def write(self, payload):
        """
        Sends the given payload to the client.
        """
        self._writer.write(payload)
        await self._writer.drain()

    async def respond_headers(self, payload_size, code=None):
        """
        Sends the response's headers - using the given payload size and the given code.
        If code param is not given, uses self.CODE.
        """
        if not code:
            code = self.CODE + 1000 # The default is (response code = request code + 1000)
        await self.write(StructWrapper(Fields.VERSION,
                                       Fields.CODE,
                                       Fields.PAYLOAD_SIZE).pack(SERVER_VERSION,
                                                                 code,
                                                                 payload_size))

    async def respond(self, payload: bytes):
        """
        Sends to the client a full response which contains:
        1) An header that was resloved by the class members and the given payload's size.
        2) The given payload.
        """
        await self.respond_headers(len(payload))
        await self.write(payload)
    
    async def respond_error(self, error_message=None):
        """
        Sends to the client a 'Server Error' response.
        """
        logging.error(error_message)
        await self.respond_headers(0, code=SERVER_ERROR_CODE)

@response_code(1000)
class RegisterResponse(Response):
    async def fetch_and_respond(self, request_headers: RequestHeaders):
        """
        Overrides abstract 'fetch_and_respond'.
        Adds the new user to the users DB, and returns the new generated client id.
        If the user already exists, sends a Server Error.
        """
        received_bytes = await self._reader.read(request_headers.payload_size)
        client_name, public_key = StructWrapper(Fields.CLIENT_NAME,
                                                Fields.PUBLIC_KEY).unpack(received_bytes)

        client_name = null_terminated_bytes_to_str(client_name)

        try:
            user = self._server.users.add_user(client_name, public_key)
            logging.info(f"New user is added - {user}")
            payload = StructWrapper(Fields.CLIENT_ID).pack(user.client_id)
            await self.respond(payload)

        except UserAlreadyExists:
            await self.respond_error(f"Got register request for an existing client name - '{client_name}'")


@response_code(1001)
class ClientListResponse(Response):
    async def fetch_and_respond(self, request_headers: RequestHeaders):
        """
        Overrides abstract 'fetch_and_respond'.
        Send to the client the client list.
        """
        if not (request_headers.payload_size == 0):
            await self.respond_error("Payload size of client list request should be 0")
            return

        payload = bytes()
        for user in self._server.users:
            payload += StructWrapper(Fields.CLIENT_ID, Fields.CLIENT_NAME).pack(user.client_id, user.client_name)

        await self.respond(payload)


@response_code(1002)
class PublicKeyResponse(Response):
    async def fetch_and_respond(self, request_headers: RequestHeaders):
        """
        Overrides abstract 'fetch_and_respond'.
        1) Resolves the client id by the given client name. If it cannot be resolved, send Server Error.
        2) Sends the user's public key to the client. 
        """
        received_bytes = await self._reader.read(request_headers.payload_size)
        client_id = StructWrapper(Fields.CLIENT_ID).unpack(received_bytes)[0]

        user = self._server.users.get_user_by_client_id(client_id)

        if not user:
            await self.respond_error(f"User with client id '{client_id}' not found")
            return

        if not user.public_key:
            await self.respond_error(f"{user} has not public key")
            return

        payload = StructWrapper(Fields.CLIENT_ID, Fields.PUBLIC_KEY).pack(client_id, user.public_key)
        await self.respond(payload)

@response_code(1003)
class MessageResponse(Response):
    async def fetch_and_respond(self, request_headers: RequestHeaders):
        """
        Overrides abstract 'fetch_and_respond'.
        1) Receives a message from the client, and adds it to the Message DB.
        2) Sends the sender client id and the generated message id to the client.
        """
        received_bytes = await self._reader.read(request_headers.payload_size)
        header_fields = (Fields.CLIENT_ID, Fields.MESSAGE_TYPE, Fields.CONTENT_SIZE)
        header_size = sum(field.size for field in header_fields)
        bytes_without_payload = received_bytes[:header_size]
        message_content = received_bytes[header_size:]

        client_id, message_type, content_size = StructWrapper(*header_fields).unpack(bytes_without_payload)

        if len(message_content) != content_size:
            await self.respond_error(f"The content length is different from the content_size. "
                                     f"content length - {len(message_content)}, content size - {content_size}")
            return 

        message = self._server.messages.add_message(client_id, request_headers.client_id, message_type, message_content)

        await self.respond(StructWrapper(Fields.CLIENT_ID, Fields.MESSAGE_ID).pack(client_id, message.message_id))

@response_code(1004)
class PullMessagesResponse(Response):
    async def fetch_and_respond(self, request_headers: RequestHeaders):
        """
        Overrides abstract 'fetch_and_respond'.
        Pulls all the waiting messages of the client from the messages DB and send them to the client.
        """
        if not (request_headers.payload_size == 0):
            await self.respond_error("Payload size of pull messages request should be 0")
            return

        payloads = []

        for message in self._server.messages.get_client_messages(request_headers.client_id):
            logging.debug(f"Sending message {message}")
            payloads.append(StructWrapper(
                Fields.CLIENT_ID,
                Fields.MESSAGE_ID,
                Fields.MESSAGE_TYPE,
                Fields.CONTENT_SIZE).pack(message.source,
                                          message.message_id,
                                          message.message_type,
                                          len(message.message_content)) + message.message_content)

        # Deleting messages from the server
        for message in self._server.messages.get_client_messages(request_headers.client_id):
            self._server.messages.delete_message(message.message_id)

        if not len(payloads):
            logging.debug(f"There are no messages for client id '{request_headers.client_id}'")

        await self.respond(b"".join(payloads))
