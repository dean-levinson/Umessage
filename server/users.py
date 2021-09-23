import logging
import uuid

from server_exceptions import UserAlreadyExists

class Users(object):
    def __init__(self):
        self._users = {} # type: dict[str, User]

    def add_user(self, client_name, public_key=None):
        if client_name in self._users:
            raise UserAlreadyExists(client_name)
        
        new_user = User(client_name, pubkey=public_key)
        self._users[client_name] = new_user
        return new_user
    
    def get_user_by_client_name(self, client_name):
        return self._users[client_name]

    def get_user_by_client_id(self, client_id):
        for user in self:
            if user.client_id == client_id:
                return user
        logging.warning(f"Couldn't find client id {client_id}")
    
    def __iter__(self):
        return iter(self._users.values())

    @staticmethod
    def generate_client_id():
        return uuid.uuid4().bytes
        
class User(object):
    def __init__(self, client_name, client_id=None, pubkey=None):
        self.client_name = client_name
        self.client_id = client_id
        if not self.client_id:
            self.client_id = Users.generate_client_id()
        self.pubkey = pubkey

    def __str__(self):
        return f"<Client name=\"{self.client_name}\">"