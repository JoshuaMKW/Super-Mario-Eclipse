

## Implementation of a yaz0 decoder/encoder in Python, by Yoshi2
## Using the specifications in http://www.amnoid.de/gc/yaz0.txt

import struct
import os
import re
import hashlib
import math

from timeit import default_timer as time
from io import BytesIO
from cStringIO import StringIO

from streamtools import read_ubyte, read_uint32, write_ubyte, write_uint32

class YAZ0(object):
    def __init__(self, inputobj, outputobj = None, compress: bool = False):
        
        self.compressFlag = compress
        self._fileobj = inputobj
        
        if outputobj == None:
            self._output = BytesIO()
        else:
            self._output = outputobj
        
        # A way to discover the total size of the input data that
        # should be compatible with most file-like objects.
        self._fileobj.seek(0, 2)
        self._maxsize = self._fileobj.tell()
        self._fileobj.seek(0)
        
        
        if self.compressFlag == False:
            self.header = self._fileobj.read(4)
            if self.header != b"Yaz0":
                raise RuntimeError(f"File is not Yaz0-compressed! Header: {self.header}")
            
            self.decompressedSize = read_uint32(self._fileobj)
            self._fileobj.seek(8) # Unused data
        
        else:
            self._output.write(b"Yaz0")
            write_uint32(self._output, self.maxsize)
            self._output.write(b"\x00"*8)

    @property
    def data(self):
        return self._fileobj

    @property
    def isCompressed(self):
        _pos = self._fileobj.tell()
        self._fileobj.seek(0)
        compressed = self._fileobj.read(4) == b"Yaz0"
        self._fileobj.seek(_pos)
        return compressed
        
    def decompress(self):
        self.assert_compressed()
        
        currentSize = 0
        while currentSize < self.decompressedSize:
            # The codebyte tells us what we need to do for the next 8 steps.
            codeByte = self._fileobj.read(1)
            
            assert self._fileobj.tell() < self.maxsize, "EOF reached, caused by bad size info or invalid data"
            
            for bit_number, bit in enumerate(self.__bit_iter__(codeByte)):
                if bit is True:
                    ## The bit is set to 1, we do not need to decompress anything. 
                    ## Write the data to the self._output.

                    assert self._output.tell() < self.decompressedSize, "Decompressed size reached prematurely"
                    
                    self._output.write(self._fileobj.read(1))
                    
                else:
                    assert self._output.tell() < self.decompressedSize, "Decompressed size reached prematurely when flagged to decompress a new block"
                    
                    ## Time to work some decompression magic. The next two bytes will tell us
                    ## where we find the data to be copied and how much data it is.
                    byte1 = read_ubyte(self._fileobj)
                    byte2 = read_ubyte(self._fileobj)
                    
                    byteCount = byte1 >> 4
                    byte1_lowerNibble = byte1 & 0xF
                    
                    if byteCount == 0:
                        # We need to read a third byte which tells us 
                        # how much data we have to read.
                       
                        byteCount = read_ubyte(self._fileobj) + 0x12
                    else:
                        byteCount += 2
                        
                    moveDistance = ((byte1_lowerNibble << 8) | byte2)
                    
                    _pos = self._output.tell()
                    moveTo = _pos - (moveDistance + 1)
                    
                    if moveTo < 0: 
                        raise RuntimeError(f"Invalid Seek Position: Trying to move from {_pos} to {moveTo} (MoveDistance: {moveDistance+1})")
                        
                    # We move back to a position that has the data we will copy to the front.
                    self._output.seek(moveTo)
                    toCopy = self._output.read(byteCount)
                    
                    if len(toCopy) < byteCount:
                        # The data we have read is less than what we should read, 
                        # so we will repeat the data we have read so far until we 
                        # have reached the bytecount.
                        newCopy = toCopy
                        diff = byteCount - len(toCopy)
                        
                        # Append full copy of the current string to our new copy
                        for i in range(diff // len(toCopy)):
                            newCopy += toCopy
                        
                        # Append the rest of the copy to the new copy
                        tocopy = newcopy + toCopy[:(diff % len(toCopy))]
                    
                    self._output.seek(_pos)
                    
                    if self.decompressedSize - _pos < byteCount:
                        diff = self.decompressedSize - _pos
                        oldCopy = map(hex, map(ord, toCopy))
                        print ("Difference between current position and "
                               "decompressed size is smaller than the length "
                               "of the current string to be copied.")

                        assert diff >= 0, "Yaz0 decompression has already surpassed the compressed size"

                        if diff == 0:
                            toCopy = b""
                            print ("toCopy string (content: '{0}') has been cleared because "
                                   "current position is close to decompressed size.".format(oldCopy))
                        else:
                            toCopy = toCopy[:diff]
                            print(len(toCopy), diff)
                            print ("toCopy string (content: '{0}') has been shortened to {1} byte/s "
                                   "because current position is close to decompressed size.".format(oldCopy,
                                                                                                    diff))
                        
                    
                    self._output.write(toCopy)
                       
                    
        print("Finished the decompression")
        print(f"EndPos: {output.tell()}, Uncompressed Size: {self.decompressedSize}")
        
        return self._output
    
    
    # To do: 
    # 1) Optimization
    # 2) Better compression
    # 3) Testing under real conditions 
    #    (e.g. replace a file in a game with a file compressed with this method)
    def compress(self, compressLevel: int = 0, advanced: bool = False):
        self.assert_decompressed()
        
        if compressLevel >= 10 or compressLevel < 0:
            raise RuntimeError("CompressionLevel is limited to 0-9.")
        
        # compressLevel can be one of the values from 0 to 9.
        # It will reduce the area in which the method will look
        # for matches and speed up compression slightly.
        compressRatio = (1/10.0) * (compressLevel+1)
        maxSearch = 4096 - 1
        adjustedSearch = int(maxSearch*compressRatio)
        adjustedMaxBytes = int(math.ceil(15*compressRatio+2))
        
        # The advanced flag will allow the use of a third byte,
        # enabling the method to look for matches that are up to 
        # 256 bytes long. NOT IMPLEMENTED YET
        
        if not advanced:
            while self._fileobj.tell() < self._maxsize:
                buffer = BytesIO()
                codeByte = 0
                
                # Compressing data near the end can be troublesome, so we will just read the data
                # and write it uncompressed. Alternatively, checks can be added to
                # the code further down, but that requires more work and testing.
                #if self._maxsize - self._fileobj.tell() <= 17*8:
                #    print "Left: {0} bytes".format(self._maxsize - self._fileobj.tell())
                #    leftover = self._fileobj.read(8).ljust(8,chr(0x00))
                #    codeByte = 0xFF
                #    buffer.write(leftover) 
                    
                    
                #else:
                for i in range(8):
                    # 15 bytes can be stored in a nibble. The decompressor will
                    # read 15+2 bytes, possibly to account for the way compression works.
                    maxBytes = adjustedMaxBytes
                    
                    # Store the current file pointer for reference.
                    currentPos = self._fileobj.tell()
                    
                    # Adjust maxBytes if we are close to the end.
                    if self._maxsize - currentPos < maxBytes:
                        maxBytes = self._maxsize - currentPos
                        print "Maxbytes adjusted to", maxBytes
                    
                    # Calculate the starting position for the search
                    searchPos = currentPos-adjustedSearch
                    
                    # Should the starting position be negative, it will be set to 0.
                    # We will also adjust how much we need to read.
                    if searchPos < 0:
                        searchPos = 0
                        realSearch = currentPos
                    else:
                        realSearch = adjustedSearch
                    
                    # toSearch will be the string (up to 2**12 long) in which
                    # we will search for matches of the pattern.
                    pattern = self._fileobj.read(maxBytes)
                    self._fileobj.seek(searchPos)
                    toSearch = self._fileobj.read(realSearch)
                    self._fileobj.seek(currentPos + len(pattern))
                    
                    index = toSearch.rfind(pattern)
                    
                    # If a match hasn't been found, we will start a loop in which we
                    # will steadily reduce the length of the pattern, increasing the chance
                    # of finding a matching string. The pattern needs to be at least 3 bytes
                    # long, otherwise there is no point in trying to compress it.
                    # (The algorithm uses at least 2 bytes to represent such patterns)
                    while index == -1 and maxBytes > 3:
                        self._fileobj.seek(currentPos)
                        
                        maxBytes -= 1
                        pattern = self._fileobj.read(maxBytes)
                        
                        if len(pattern) < maxBytes:
                            maxBytes = len(pattern) 
                            print "adjusted pattern length"
                            
                        index = toSearch.rfind(pattern)
                    
                    if index == -1 or maxBytes <= 2:
                        # No match found. Read a byte and append it to the buffer directly.
                        self._fileobj.seek(currentPos)
                        byte = self._fileobj.read(1)
                        
                        # At the end of the file, read() will return an empty string.
                        # In that case we will set the byte to the 0 character.
                        # Hopefully, a decompressor will check the uncompressed size
                        # of the file and remove any padding bytes past this position.
                        if len(byte) == 0:
                            #print "Adding padding"
                            byte = chr(0x00)
                        
                        buffer.write(byte)
                        
                        # Mark the bit in the codebyte as 1.
                        codeByte = (1 << (7-i)) | codeByte
                        
                    else:
                        # A match has been found, we need to calculate its index relative to
                        # the current position. (RealSearch stores the total size of the search string,
                        # while the index variable holds the position of the pattern in the search string)
                        relativeIndex = realSearch - index 
                        
                        # Create the two descriptor bytes which hold the length of the pattern and
                        # its index relative to the current position.
                        # Marking the bit in the codebyte as 0 isn't necessary, it will be 0 by default.
                        byte1, byte2 = self.__build_byte__(maxBytes-2, relativeIndex-1)
                        
                        buffer.write(chr(byte1))
                        buffer.write(chr(byte2))
            
                # Now that everything is done, we will append the code byte and
                # our compressed data from the buffer to the output.
                self._output.write(chr(codeByte))
                self._output.write(buffer.getvalue())
        else:
            raise RuntimeError("Advanced compression not implemented yet.")
        
        return self._output
                    
    def __build_byte__(self, byteCount, position):
        if position >= 2**12:
            raise RuntimeError("{0} is outside of the range for 12 bits!".format(position))
        if byteCount > 0xF:
            raise RuntimeError("{0} is too much for 4 bits.".format(byteCount))
        
        positionNibble = position >> 8
        positionByte = position & 0xFF
        
        byte1 = (byteCount << 4) | positionNibble
        
        return byte1, positionByte
        
        
    # A simple iterator for iterating over the bits of a single byte
    def __bit_iter__(self, byte):
        for i in range(8):
            result = (byte << i) & 0x80
            yield result != 0



#
#    Helper Functions for easier usage of
#    the compress & decompress methods of the module.
#

# Take a compressed string, decompress it and return the
# results as a string. 
def decompress(string):
    stringObj = StringIO(string)
    yaz0obj = yaz0(stringObj, compress = False)
    return yaz0obj.decompress().getvalue()

# Take a file-like object, decompress it and return the
# results as a StringIO object.
def decompress_fileobj(fileobj):
    yaz0obj = yaz0(fileobj, compress = False)
    return yaz0obj.decompress()

# Take a file name and decompress the contents of that file. 
# If outputPath is given, save the results to a file with
# the name defined by outputPath, otherwise return the results
# as a StringIO object.
def decompress_file(filenamePath, outputPath = None):
    with open(filenamePath, "rb") as fileobj:
        yaz0obj = yaz0(fileobj, compress = False)
        
        result = yaz0obj.decompress()
        
        if outputPath != None:
            with open(outputPath, "wb") as output:
                output.write(result.getvalue())
            
            result = None
            
    return result


# Take an uncompressed string, compress it and
# return the results as a string.
def compress(string, compressLevel = 9):
    stringObj = StringIO(string)
    yaz0obj = yaz0(stringObj, compress = True)
    return yaz0obj.compress(compressLevel).getvalue()

# Take a file-like object, compress it and
# return the results as a StringIO object.
def compress_fileobj(fileobj, compressLevel = 9):
    yaz0obj = yaz0(fileobj, compress = True)
    return yaz0obj.compress(compressLevel)

# Take a file name and compress the contents of that file.
# If outputPath is not None, write the results to a file
# with the name defined by outputPath, otherwise return
# results as a StringIO object.
def compress_file(filenamePath, outputPath = None, compressLevel = 9):
    with open(filenamePath, "rb") as fileobj:
        yaz0obj = yaz0(fileobj, compress = True)
        
        result = yaz0obj.compress(compressLevel)
        
        if outputPath != None:
            with open(outputPath, "wb") as output:
                output.write(result.getvalue())
            
            result = None
            
    return result

def assert_compressed(self):
    _pos = self._fileobj.tell()
    self._fileobj.seek(0)
    assert self._fileobj.read(4) == b"Yaz0", "File is decompressed, when it should be compressed"
    self._fileobj.seek(_pos)

def assert_decompressed(self):
    _pos = self._fileobj.tell()
    self._fileobj.seek(0)
    assert self._fileobj.read(4) != b"Yaz0", "File is compressed, when it should be decompressed"
    self._fileobj.seek(_pos)



if __name__ == "__main__":
    compress = True
        
    if not compress:
        fileobj = open("compressed.dat", "rb")
        yazObj = yaz0(fileobj)
        output = yazObj.decompress()
        fileobj.close()
        
        writefile = open("decompressed.dat", "wb")
        writefile.write(output.getvalue())
        writefile.close()
        
    else:
        start = time()
        fileobj = open("decompressed.dat", "rb")
        yazObj = yaz0(fileobj, compress = True)
        output = yazObj.compress(compressLevel = 9)
        fileobj.close()
        
        writefile = open("compressed.dat", "wb")
        writefile.write(output.getvalue())
        writefile.close()
        
        print "Time taken: {0} seconds".format(time()-start)
        