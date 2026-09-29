
#ifndef _DOCWORDFILEPARSER_H_
#define _DOCWORDFILEPARSER_H_

#include <memory>     // std::shared_ptr

class DocWordFileParser {
   private:
      std::shared_ptr<char> _buffer; 
      int _fd;
      size_t _bufferSize;
      char * _bufferStart;
      char * _bufferEnd;
      char * _bufferReadCursor;
      char * _bufferWriteCursor;
      char * _lowWaterMark;
      //invariant: _bufferStart <= _bufferReadCursor <= _lowWaterMark <= _bufferWriteCursor <= _bufferEnd
      void _makeSpace();
      void _readMoreBytes();
      size_t _bufferFreeBytes() const;
      size_t _bufferUsedBytes() const;
      size_t _bufferStaleBytes() const;
      bool _needToReadMoreBytes() const;
   public:
      DocWordFileParser(std::string const &docword_filepath, const int bufferSize = (1024*1024*4)+10);
      ~DocWordFileParser();
      bool readThreeIntegers(int *a);
      bool readOneInteger(int *a);
};

#endif // _DOCWORDFILEPARSER_H_

