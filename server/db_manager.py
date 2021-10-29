import sqlite3
from functools import wraps

from server_consts import DB_FILE_PATH

class DBManaged(object):
    _TABLE_NAME = None
    _TABLE_PARAMS = None

    def __init__(self) -> None:
        self._in_transaction = False
        self._setup_db()

    def _setup_db(self):
        """
        Setup the database. Creates the Database if it doesn't already exist.
        """
        self._create_if_not_exist()

    def db_transaction(func):
        """
        Decorator that wraps every method that interacts with the db.
        Before the intraction - connect to the database and setup the cursor.
        After the intraction - Commits the changes and closes the connection.
        """
        @wraps(func)  
        def wrapper(self, *args, **kwargs):
            if self._in_transaction:
                return func(self, *args, **kwargs)

            self._in_transaction = True
            self._conn = sqlite3.connect(DB_FILE_PATH)
            self._conn.text_factory = bytes
            self._cur = self._conn.cursor()

            try:
                result = func(self, *args, **kwargs)

            finally:
                self._cur.close()
                self._conn.commit()
                self._conn.close()
                self._in_transaction = False

            return result

        return wrapper

    @db_transaction
    def _create_if_not_exist(self):
        """
        Creates the table if it doesn't already exist.
        """
        self._cur.execute(f"""
        create table if not exists {self._TABLE_NAME}({", ".join(self._TABLE_PARAMS)})
        """)
