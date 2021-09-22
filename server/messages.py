class Messages(object):
    def __init__(self):
        self._messages = {}

    def add_message(self, to_client, from_client, message_type, content):
        message = Message(to_client, from_client, message_type, content)
        self._messages.setdefault(to_client, []).append(message)

    def get_client_messages(self, client_id):
        return self._messages[client_id]

class Message(object):
    def __init__(self, to_client, from_client, message_type, content):
        self.target = to_client
        self.source = from_client
        self.message_type = message_type
        self.content = content
