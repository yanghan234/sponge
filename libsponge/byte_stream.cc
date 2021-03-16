#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity): _capacity(capacity) { }

size_t ByteStream::write(const string &data) {
    size_t cnt = 0;
    while ( _buffer.size() < _capacity && cnt < data.size() )
        _buffer.push_back(data[cnt++]);
    _total_write += cnt;
    return cnt;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t cnt = 0;
    string res;
    while ( cnt < len && cnt < _buffer.size() )
        res += _buffer[cnt++];
    return res;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t cnt = 0, bufsize = _buffer.size();
    while ( cnt < len && cnt < bufsize )
    {
        _buffer.pop_front();
        cnt++;
    }
    _total_read += cnt;
 }

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    std::string res = peek_output(len);
    pop_output(len);
    return res;
}

void ByteStream::end_input() { eof_flag = true; }

bool ByteStream::input_ended() const { return eof_flag; }

size_t ByteStream::buffer_size() const { return _buffer.size(); }

bool ByteStream::buffer_empty() const { return _buffer.empty(); }

bool ByteStream::eof() const { return input_ended() && buffer_empty(); }

size_t ByteStream::bytes_written() const { return _total_write; }

size_t ByteStream::bytes_read() const { return _total_read; }

size_t ByteStream::remaining_capacity() const { return _capacity-_buffer.size(); }
