#ifndef COMPRESSION_HPP_INCLUDED
#define COMPRESSION_HPP_INCLUDED

#include <string>
#include <memory>
#include <stdint.h>

#include "util/UtilType.hpp"

namespace trillek {
namespace util {

typedef std::basic_string<
    unsigned char,
    std::char_traits<unsigned char>,
    std::allocator<unsigned char>
> DataString;

class CompressionMethod {
public:
    CompressionMethod() {}
    virtual ~CompressionMethod() {};
    virtual bool CompressStart() = 0;
    virtual bool CompressEnd() = 0;
    virtual bool CompressData(DataString) = 0;
    virtual bool CompressHasOutput() = 0;
    virtual DataString CompressGetOutput() = 0;
};
class DecompressionMethod {
public:
    DecompressionMethod() {}
    virtual ~DecompressionMethod() {};

    virtual bool DecompressStart() = 0;
    virtual bool DecompressEnd() = 0;
    virtual bool DecompressData(DataString) = 0;
    virtual bool DecompressHasOutput() = 0;
    virtual DataString DecompressGetOutput() = 0;
};

namespace algorithm {

    struct Huffman {
        uint16_t fast[512];
        uint16_t firstcode[16];
        uint32_t maxcode[17];
        uint16_t firstsymbol[16];
        uint8_t size[288];
        uint16_t value[288];

        void_er Build(uint8_t *sizelist, uint32_t num);
    };

    struct BitStreamDecoder {
        DataString indata;
        DataString::size_type inpos;
        uint32_t num_bits;
        uint32_t bit_buffer;

        BitStreamDecoder();

        /** \brief put data into the stream
         */
        void_er AppendData(const DataString & in);

        /** \brief get a byte from the stream
         */
        ErrorReturn<uint8_t> ReadByte();
        // used for bit buffer filling
        void_er LoadByte();
        void_er LoadFull();

        /** \brief get bits out of the stream
         */
        ErrorReturn<uint32_t> GetBits(uint32_t n);

    };

    enum class InflateStateNumber : uint32_t {
        HEADER,
    };

    class Inflate : public DecompressionMethod {
    public:
        Inflate();
        ~Inflate();
        bool DecompressStart();
        bool DecompressEnd();
        bool DecompressData(DataString);
        bool DecompressHasOutput();
        DataString DecompressGetOutput();
    protected:
        ErrorReturn<uint16_t> HuffmanDecode(const Huffman&);
        void_er UncompressedBlock();
        void_er HuffmanBlock();

        BitStreamDecoder instream;
        DataString outdata;
        unsigned long outpos;
        bool errored;
        void_er error_state;
        Huffman length, distance;
        InflateStateNumber readstate;
    };

} // algorithm

} // util
} // trillek

#endif
