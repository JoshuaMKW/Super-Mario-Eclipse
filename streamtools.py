import struct
from io import BytesIO, IOBase

def read_sbyte(f):
    return struct.unpack("b", f.read(1))[0]

def write_sbyte(f, val):
    f.write(struct.pack("b", val))

def read_sint16(f):
    return struct.unpack(">h", f.read(2))[0]

def write_sint16(f, val):
    f.write(struct.pack(">h", val))

def read_sint32(f):
    return struct.unpack(">i", f.read(4))[0]

def write_sint32(f, val):
    f.write(struct.pack(">i", val))

def read_ubyte(f):
    return struct.unpack("B", f.read(1))[0]

def write_ubyte(f, val):
    f.write(struct.pack("B", val))

def read_uint16(f):
    return struct.unpack(">H", f.read(2))[0]

def write_uint16(f, val):
    f.write(struct.pack(">H", val))

def read_uint32(f):
    return struct.unpack(">I", f.read(4))[0]

def write_uint32(f, val):
    f.write(struct.pack(">I", val))

def read_f32(f):
    return struct.unpack(">f", f.read(4))[0]

def write_f32(f, val):
    f.write(struct.pack(">f", val))

def read_double(f):
    return struct.unpack(">d", f.read(4))[0]

def write_double(f, val):
    f.write(struct.pack(">d", val))

def read_bool(f, vSize=1):
    return struct.unpack("B", f.read(vSize))[0] > 0

def write_bool(f, val, vSize=1):
    if val is True: f.write(b'\x00'*(vSize-1) + b'\x01')
    else: f.write(b'\x00' * vSize)

def get_alignment(number, align: int):
    if number % align != 0:
        return align - (number % align)
    else:
        return 0

def stream_size(obj, ofs: int = 0) -> int:
    if hasattr(obj, "getbuffer"):
        return len(obj.getbuffer()) + ofs
    elif hasattr(obj, "tell") and hasattr(obj, "seek"):
        _pos = obj.tell()
        obj.seek(0, 2)
        _size = obj.tell()
        obj.seek(_pos)
        return _size + ofs
    else:
        raise NotImplementedError(f"Getting the stream size of class {type(obj)} is unsupported")

def align_byte_size(obj, alignment: int, fillchar="00"):
    if isinstance(obj, bytes):
        obj += bytes.fromhex(fillchar * get_alignment(len(obj), alignment))
    elif isinstance(obj, bytearray):
        obj.append(bytearray.fromhex(fillchar * get_alignment(len(obj), alignment)))
    elif issubclass(type(obj), IOBase):
        _size = stream_size(obj)
        obj.write(bytes.fromhex(fillchar * get_alignment(_size, alignment)))
    else:
        raise NotImplementedError(f"Aligning the size of class {type(obj)} is unsupported")