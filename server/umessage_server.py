import asyncio
import logging
from typing import Type

from users import Users
from messages import Messages
import server_requests
import server_responses
from server_exceptions import NoSuchUser

class UMessageServer(object):    
    def __init__(self) -> None:
        self.users = Users()
        self.messages = Messages()

    async def handle_client(self, reader, writer):
        """
        Called as a callback after creating a connection with the client.
        reader, writer - Represents a reader, writer object that provides APIs to read and write data
        from the IO stream.
        While the connection is established, serves the client - 
        1) Waits for client request.
        2) Parses the request's headers and updates the user's last seen.
        3) Routes it the to the corresponding Response handler that fetches the rest of the request
        and respond accordingly. 
        """
        try: 
            peer = writer.get_extra_info('peername')
            logging.info(f"Client connected from {peer}")
            while True:
                request_headers = server_requests.RequestHeaders(reader)
                await request_headers.fetch()

                # Update user's last seen if is already exist.
                try:
                    user = self.users.update_user_last_seen(self.users.get_user_by_client_id(request_headers.client_id))
                    logging.debug(f"Got request {request_headers} from {user}")
                except NoSuchUser: # User hasn't created yet.
                    logging.debug(f"Got request {request_headers}")
                
                try:
                    response_cls = server_responses.RESPONSES[request_headers.code] # type: Type[server_responses.Response]
                except KeyError: # No such code
                    logging.error("Got unknown request code from client. ignoring...")
                    continue

                response = response_cls(self, reader, writer)
                await response.fetch_and_respond(request_headers)

        except (asyncio.streams.IncompleteReadError, ConnectionResetError):
            logging.info(f"Client {peer} as disconnected")