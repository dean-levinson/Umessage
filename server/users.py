import uuid
import datetime
from functools import wraps

from server_exceptions import UserAlreadyExists, DBError, NoSuchUser
from db_manager import DBManaged
        
class User(object):
    """
    Dummy object of User, just for representation.
    """
    def __init__(self, client_id: bytes, client_name: str, public_key: bytes, last_seen: datetime):
        self.client_id = client_id
        self.client_name = client_name
        self.public_key = public_key
        self.last_seen = last_seen

    def __str__(self):
        return f"<Client name=\"{self.client_name}\", last_seen=\"{self.last_seen}\">"
        
class Users(DBManaged):
    _TABLE_NAME = "USERS"
    _TABLE_PARAMS = ["ID varchars(16) NOT NULL PRIMARY KEY",
                     "Name varchars(255)",
                     "PublicKey varchars(160)",
                     "LastSeen timestamp"]

    def __init__(self) -> None:
        super().__init__()

    def add_user(self, client_name: str, public_key: bytes):
        """
        Adds a new user to the Database and returns a dummy obj of User.
        """
        if self.is_user_exist(client_name):
            raise UserAlreadyExists(client_name)
        
        new_user = User(self.generate_client_id(), client_name, public_key, datetime.datetime.now())
        self._add_user_to_db(new_user)
        return new_user

    def is_user_exist(self, client_name: str):
        """
        Returns a boolean that indicates weather the given client_name exists in the Database.
        """
        try:
            self.get_user_by_client_name(client_name)
        except NoSuchUser:
            return False
        
        return True

    @DBManaged.db_transaction
    def get_user_by_client_name(self, client_name: str):
        """
        Search for a user with the given client name in the Database. 
        If found more than one, raises DBError,
        Else if not found, raises NoSuchUser exception,
        Else, Returns a dummy User obj.
        """
        results = self._cur.execute(f"SELECT ID, Name, PublicKey, LastSeen FROM \
         {self._TABLE_NAME} WHERE Name = ?;", (client_name, )).fetchall()

        if len(results) > 1:
            raise DBError(f"Found more than 1 client with client_name {client_name}")

        if len(results) == 0:
            raise NoSuchUser(f"No such client {client_name}")

        return self._get_user_from_db_result(results[0])

    @DBManaged.db_transaction
    def get_user_by_client_id(self, client_id: bytes):
        """
        Searches for a user with the given client id in the Database. 
        If found more than one, raises DBError,
        Else if not found, raises NoSuchUser exception,
        Else, Returns a dummy User obj.
        """
        results = self._cur.execute(f"SELECT ID, Name, PublicKey, LastSeen FROM {self._TABLE_NAME} WHERE ID = ?;", (client_id, )).fetchall()

        if len(results) > 1:
            raise DBError(f"Found more than 1 client with client_id {client_id}")

        if len(results) == 0:
            raise NoSuchUser(f"No such client {client_id}")

        return self._get_user_from_db_result(results[0])

    @DBManaged.db_transaction
    def _add_user_to_db(self, user: User):
        """
        Adds a new user to the Database using the given client name and public key.
        """
        self._cur.execute(f"INSERT INTO '{self._TABLE_NAME}' VALUES(?, ?, ?, ?);",
                          (user.client_id, user.client_name, user.public_key, user.last_seen))
    
    @DBManaged.db_transaction
    def __iter__(self):
        """
        Returns an iterator[User] of all the users in the Database.
        """
        all_results = self._cur.execute(f"SELECT ID, Name, PublicKey, LastSeen FROM {self._TABLE_NAME};").fetchall()
        return iter([self._get_user_from_db_result(result) for result in all_results])


    @DBManaged.db_transaction
    def update_user_last_seen(self, user: User):
        """
        Updates the given user's last seen in the database with the current time.
        Returns the new dummy User obj.
        """
        if not self.is_user_exist(user.client_name):
            raise NoSuchUser()

        self._cur.execute(f"UPDATE {self._TABLE_NAME} SET LastSeen = ? WHERE ID = ?", (datetime.datetime.now(),
                                                                                       user.client_id))

        return self.get_user_by_client_id(user.client_id)

    @staticmethod
    def generate_client_id():
        return uuid.uuid4().bytes

    @staticmethod
    def _get_user_from_db_result(db_result: tuple):
        """
        Creates a dummy User obj from sqlite3 tuple result that indicates a row in the table.
        """
        client_id, client_name, public_key, last_seen = db_result
        return User(client_id, client_name.decode(), public_key, datetime.datetime.fromisoformat(last_seen.decode()))
    