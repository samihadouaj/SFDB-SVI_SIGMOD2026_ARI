#include "DocWordFileParser.h"

#include <cstdlib> // strtol
#include <cstring>
#include <errno.h> // errno
#include <fcntl.h> // open(), posix_fadvise()
#include <string>
#include <unistd.h> // read()

DocWordFileParser::DocWordFileParser(std::string const &docword_filepath, const int bufferSize)
   :_buffer((char*) malloc(sizeof(char) * bufferSize), free), 
    _fd(open(docword_filepath.c_str(), O_RDONLY)),
    _bufferSize(bufferSize), 
    _bufferStart(_buffer.get()),
    _bufferEnd(_buffer.get()+bufferSize),
    _bufferReadCursor(_buffer.get()),
    _bufferWriteCursor(_buffer.get()),
    _lowWaterMark(_buffer.get())
{
   if (_fd != -1) {
      posix_fadvise(_fd, 0, 0, POSIX_FADV_SEQUENTIAL); 
   }
}

DocWordFileParser::~DocWordFileParser()
{
   if (_fd != -1) {
      close(_fd);
   }
}

void DocWordFileParser::_readMoreBytes()
{
   if (_bufferReadCursor<_lowWaterMark) {
      return; // not yet
   }
   if (_bufferFreeBytes()==0) {
      return; // not enough space
   }
   size_t bytes_read = read(_fd,  _bufferWriteCursor, _bufferFreeBytes());
   //std::cout << "Reading " << ((1.0*bytes_read) / (1024*1024)) << " MBs ..." << std::endl;
   if (bytes_read > 0) {
      _bufferWriteCursor += bytes_read;
      _lowWaterMark = _bufferReadCursor;
      for (auto i=(_bufferWriteCursor-1); i>=_bufferReadCursor; i--)
      {
         if ((*i)=='\n') {
            _lowWaterMark = i;
            break;
         }
      }
   }
}

void DocWordFileParser::_makeSpace()
{
   if (_bufferReadCursor == _bufferStart)
      return;
   std::memcpy(_bufferStart, _bufferReadCursor, _bufferUsedBytes());
   _bufferWriteCursor -= _bufferStaleBytes();
   _lowWaterMark -= _bufferStaleBytes();
   _bufferReadCursor = _bufferStart;
}

size_t DocWordFileParser::_bufferFreeBytes() const
{
   return _bufferEnd-_bufferWriteCursor;
}

size_t DocWordFileParser::_bufferUsedBytes() const
{
   return _bufferWriteCursor-_bufferReadCursor;
}

size_t DocWordFileParser::_bufferStaleBytes() const
{
   return _bufferReadCursor - _bufferStart;
}

bool DocWordFileParser::_needToReadMoreBytes() const
{
   return (_bufferReadCursor>=_lowWaterMark);
}

bool DocWordFileParser::readOneInteger(int *a)
{
   if (_needToReadMoreBytes()) {
      _makeSpace();
      _readMoreBytes();
   }

   char * pEnd;
   char * pStart = _bufferReadCursor;

   if ((pStart>=_bufferWriteCursor)||(pStart>=_lowWaterMark)) {
      return false;
   }
   errno = 0;
   a[0] = (int) strtol (pStart,&pEnd,10);
   if ((pStart==pEnd)||(errno != 0)) { 
      return false;
   }
   pStart = pEnd+1;
   _bufferReadCursor = pStart;
   return true;
}

bool DocWordFileParser::readThreeIntegers(int *a)
{
   if (_needToReadMoreBytes()) {
      _makeSpace();
      _readMoreBytes();
   }

   char * pEnd;
   char * pStart = _bufferReadCursor;
   for (int i=0; i<3; i++)
   {
      if ((pStart>=_bufferWriteCursor)||(pStart>=_lowWaterMark)) {
         return false;
      }
      errno = 0;
      a[i] = (int) strtol (pStart,&pEnd,10);
      if ((pStart==pEnd)||(errno != 0)) { 
         return false;
      }
      pStart = pEnd+1;
   }
   _bufferReadCursor = pStart;
   return true;
}
