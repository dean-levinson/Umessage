import asyncio
from server_consts import *
from struct_wrapper import StructWrapper

class RequestFetcher(object):
    def __init__(self, reader: asyncio.StreamReader):
        self.reader = reader

    def fetch():
        raise NotImplementedError("Abstract class")

class RequestHeaders(RequestFetcher): 
    def __init__(self, reader: asyncio.StreamReader):
        super().__init__(reader)
        self.client_id = None
        self.version = None
        self.code = None
        self.payload_size = None

    async def fetch(self):
        struct_wrapper = StructWrapper(Fields.CLIENT_ID, 
                                       Fields.VERSION,
                                       Fields.CODE,
                                       Fields.PAYLOAD_SIZE)

        received_bytes = await self.reader.readexactly(struct_wrapper.size())

        self.client_id, self.version, self.code, self.payload_size = struct_wrapper.unpack(received_bytes)

    def __str__(self):
        return f"<RequestHeaders client_id={self.client_id}, version={self.version}, " + \
        f"code={self.code}, payload_size={self.payload_size}>"