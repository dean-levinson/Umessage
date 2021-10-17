import uuid
import datetime
from functools import wraps
from server.server_exceptions import DBError, NoSuchUser

from server_exceptions import UserAlreadyExists
from db_manager import DBManaged
        
class Users(DBManaged):
    _TABLE_NAME = "USERS"
    _TABLE_PARAMS = ["ID varchars(16) NOT NULL PRIMARY KEY",
                     "Name varchars(255)",
                     "PublicKey varchars(160)",
                     "LastSeen timestamp"]

    def __init__(self):
        self._setup_db()

    def add_user(self, client_name, public_key):
        if self.is_client_exist(client_name):
            raise UserAlreadyExists(client_name)
        
        new_user = User(self.generate_client_id(), client_name, public_key)
        self._add_user_to_db(new_user.client_id, new_user.client_name, new_user.public_key)
        return new_user

    def is_client_exist(self, client_name):
        try:
            self.get_user_by_client_name(client_name)
        except NoSuchUser:
            return False
        
        return True

    @DBManaged.db_transaction
    def get_user_by_client_name(self, client_name):
        results = self._cur.execute(f"SELECT ID, Name, PublicKey FROM {self._TABLE_NAME} WHERE Name = ?;", (client_name, )).fetchall()
        if len(results) > 1:
            raise DBError(f"Found more than 1 client with client_name {client_name}")

        if len(results) == 0:
            raise NoSuchUser(f"No such client {client_name}")

        client_id, client_name, public_key = results[0]
        return User(client_id, client_name, public_key)

    @DBManaged.db_transaction
    def get_user_by_client_id(self, client_id):
        results = self._cur.execute(f"SELECT ID, Name, PublicKey FROM {self._TABLE_NAME} WHERE ID = ?;", (client_id, )).fetchall()
        if len(results) > 1:
            raise DBError(f"Found more than 1 client with client_id {client_id}")

        if len(results) == 0:
            raise NoSuchUser(f"No such client {client_id}")

        client_id, client_name, public_key = results[0]
        return User(client_id, client_name, public_key)

    @DBManaged.db_transaction
    def _add_user_to_db(self, client_id, client_name, public_key):
        self._cur.execute(f"INSERT INTO '{self._TABLE_NAME}' VALUES(?, ?, ?, ?);",
                          (client_id, client_name, public_key, datetime.datetime.now()))
    
    @DBManaged.db_transaction
    def __iter__(self):
        all_results = self._cur.execute(f"SELECT ID, Name, PublicKey FROM {self._TABLE_NAME};").fetchall()
        return iter([User(client_id, client_name, public_key) for client_id, client_name, public_key in all_results])

    @staticmethod
    def generate_client_id():
        return uuid.uuid4().bytes
        
class User(object):
    def __init__(self, client_id, client_name, public_key):
        self.client_id = client_id
        self.client_name = client_name
        self.public_key = public_key

    def __str__(self):
        return f"<Client name=\"{self.client_name}\">"
