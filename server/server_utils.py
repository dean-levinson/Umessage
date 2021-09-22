def null_terminated_bytes_to_str(bytes_obj: bytes):
    return bytes_obj.split(b'\00')[0].decode("ascii")