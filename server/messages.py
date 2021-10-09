import logging
from random import randint
from typing import List
from abc import abstractmethod

from server_exceptions import MessageNotExist
            
class Message(object):
    def __init__(self, message_id: int, to_client: bytes, from_client: bytes,
                 message_type:int , message_content: bytes):
        self.message_id  = message_id
        self.target = to_client
        self.source = from_client
        self.message_type = message_type
        self.message_content = message_content

    @staticmethod
    def generate_message_id() -> int:
        return randint(1, 2 ** 32)

    def __str__(self) -> str:
        return f"<Message id={self.message_id}, to={self.target}, from={self.source}, type={self.message_type}>"


class Messages(object):
    def __init__(self):
        self._messages = {} # type: dict[int, Message]
        self._client_to_message_id = {} # type: dict[str, list[int]]

    def add_message(self, to_client: bytes, from_client: bytes, message_type: int, content: bytes) -> Message:
        message_id = self.find_available_message_id()
        message = Message(message_id, to_client, from_client, message_type, content)
        logging.debug(f"Saving message {message_id} -> {message}")
        self._messages[message_id] = message
        self._client_to_message_id.setdefault(to_client, []).append(message_id)

        return message

    def get_message(self, message_id: int) -> Message:
        if message_id not in self._messages:
            raise MessageNotExist(message_id)

        return self._messages[message_id]

    def delete_message(self, message_id: int) -> None:
        logging.debug(f"Deleting message {message_id}")
        message = self.get_message(message_id)
        del self._messages[message_id]
        self._client_to_message_id[message.target].remove(message_id)

    def get_client_messages(self, client_id: bytes) -> List[Message]:
        client_messages_id = self._client_to_message_id.get(client_id, [])
        return [self._messages[message_id] for message_id in client_messages_id]

    def find_available_message_id(self) -> int:
        while True:
            message_id = Message.generate_message_id()
            if message_id not in self._messages:
                return message_id