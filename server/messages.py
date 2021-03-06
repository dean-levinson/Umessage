import logging
from random import randint
from typing import List
from abc import abstractmethod

from db_manager import DBManaged
from server_exceptions import DBError
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


class Messages(DBManaged):
    _TABLE_NAME = "MESSAGES"
    _TABLE_PARAMS = ["ID INT NOT NULL PRIMARY KEY",
                     "ToClient varchars(16)",
                     "FromClient varchars(16)",
                     "Type INT",
                     "Content Blob"]

    def __init__(self) -> None:
        super().__init__()

    @DBManaged.db_transaction
    def add_message(self, to_client: bytes, from_client: bytes, message_type: int, content: bytes) -> Message:
        """
        Adds a new message to the Database. Returns a dummy Message obj of the new message.
        """
        message_id = self.find_available_message_id()
        message = Message(message_id, to_client, from_client, message_type, content)
        logging.debug(f"Saving message {message_id} -> {message}")

        self._add_message_to_db(message)
        return message

    @DBManaged.db_transaction
    def get_message(self, message_id: int) -> Message:
        """
        Searches for a message with the given message id in the Database. 
        If found more than one, raises DBError,
        Else if not found, raises MessageNotExist exception,
        Else, Returns a dummy Message obj.
        """
        results = self._cur.execute(f"SELECT * FROM {self._TABLE_NAME} WHERE ID = ?;", (message_id, )).fetchall()

        if len(results) > 1:
            raise DBError(f"Found more than 1 message with message_id {message_id}")

        if len(results) == 0:
            raise MessageNotExist(message_id)

        return self._get_message_from_db_result(results[0])

    @DBManaged.db_transaction
    def delete_message(self, message_id: int) -> None:
        """
        Deletes the message with the given message_id from the server.
        If there is no such message, raises MessageNotExist.
        """
        message = self.get_message(message_id)
    
        logging.debug(f"Deleting message {message}")

        self._cur.execute(f"DELETE FROM {self._TABLE_NAME} WHERE ID = ?;", (message_id, ))

    @DBManaged.db_transaction
    def get_client_messages(self, client_id: bytes) -> List[Message]:
        """
        Returns a list of all the messages that were sent to the given client id.
        """
        results = self._cur.execute(f"SELECT * FROM {self._TABLE_NAME} WHERE ToClient = ?;", (client_id, )).fetchall()
        return [self._get_message_from_db_result(result) for result in results]

    def is_message_exist(self, message_id: int):
        """
        Returns a boolean that indicates weather a message with the given message id exists in the Database.
        """
        try:
            self.get_message(message_id)
        except MessageNotExist:
            return False
        
        return True
    
    @DBManaged.db_transaction
    def _add_message_to_db(self, message: Message):
        """
        Adds a new message to the Database.
        """    
        self._cur.execute(f"INSERT INTO '{self._TABLE_NAME}' VALUES(?, ?, ?, ?, ?);",
                          (message.message_id, message.target, message.source,
                           message.message_type, message.message_content))

    def find_available_message_id(self) -> int:
        """
        Find a message id that is not already taken by other message.
        """
        while True:
            message_id = Message.generate_message_id()
            if not self.is_message_exist(message_id):
                return message_id

    @staticmethod
    def _get_message_from_db_result(db_result: tuple):
        """
        Creates a dummy Message obj from sqlite3 tuple result that indicates a row in the table.
        """
        message_id, to_client, from_client, message_type, message_content = db_result
        return Message(message_id, to_client, from_client, message_type, message_content)
    