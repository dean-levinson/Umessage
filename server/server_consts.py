from struct_wrapper import UintField, ArrayField


class Fields(object):
    # in Bytes
    CLIENT_NAME = ArrayField(255)
    CLIENT_ID = ArrayField(16)
    VERSION = UintField(1)
    CODE = UintField(2)
    PAYLOAD_SIZE = UintField(4)
    PUBLIC_KEY = ArrayField(160)

SERVER_VERSION = 1
SERVER_ERROR_CODE = 9000