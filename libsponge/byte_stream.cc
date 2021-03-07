#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) { buffer.resize(capacity); }

size_t ByteStream::write(const string &data) {
    size_t count = 0;
    while ( remaining_capacity() > 0 && count < data.size() )
    {
        buffer[next_write++] = data[count++];
        next_write %= buffer.capacity();
        total_write++;
    }
    return count;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    std::string res;
    if ( buffer_empty() ) return res;
    size_t bufsize = buffer_size();
    for ( size_t i = 0; i < min(len,bufsize); ++i)
    {
        res += buffer[(next_read+i)%buffer.capacity()];
    }
    return res;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t bufsize = buffer_size();
    for ( size_t i = 0; i < min(len,bufsize); ++i )
    {
        next_read++;
        next_read %= buffer.capacity();
        total_read++;
    }
 }

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    std::string res;
    if ( buffer_empty() ) set_error();
    size_t bufsize = buffer_size();
    for ( size_t i = 0; i < min(len,bufsize); ++i )
    {
        res += buffer[next_read++];
        next_read %= buffer.capacity();
        total_read++;
    }
    return res;
}

void ByteStream::end_input() { input_end_flag = true; }

bool ByteStream::input_ended() const { return input_end_flag; }

size_t ByteStream::buffer_size() const { return total_write-total_read; }

bool ByteStream::buffer_empty() const { return total_write == total_read; }

bool ByteStream::eof() const { return buffer_empty() && input_ended(); }

size_t ByteStream::bytes_written() const { return total_write; }

size_t ByteStream::bytes_read() const { return total_read; }

size_t ByteStream::remaining_capacity() const { return buffer.capacity()-buffer_size(); }
