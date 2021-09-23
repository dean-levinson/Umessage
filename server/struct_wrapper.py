import struct
 
BIG_ENDIAN = False

class Field(object):
    def __init__(self, size) -> None:
        assert(isinstance(size, int))
        self.size = size

    def format():
        raise NotImplementedError("Abstract class")

class UintField(Field):
    FIELDS_FORMAT = {
        1: 'B',
        2: 'H',
        4: 'I'
    }

    def format(self):
        if self.size in self.FIELDS_FORMAT:
            return self.FIELDS_FORMAT[self.size]

        # Retreats as an array
        return ArrayField(self.size).format()

class ArrayField(Field):
    def format(self):
        return f'{self.size}s'

class StructWrapper(object):
    def __init__(self, *args) -> None:
        self._format = self.generate_format(*args)

    def size(self):
        if not hasattr(self, "_format"):
            return 0
        
        return struct.calcsize(self._format)

    def pack(self, *args):
        args = [bytes(arg, "ascii") if isinstance(arg, str) else arg for arg in args]
        return struct.pack(self._format, *args)

    def unpack(self, received_bytes):
        return struct.unpack(self._format, received_bytes)

    @classmethod
    def generate_format(cls, *args):
        format_str = ">" if BIG_ENDIAN else "<" 
        for arg in args:
            assert(isinstance(arg, Field))
            format_str += arg.format()

        return format_str