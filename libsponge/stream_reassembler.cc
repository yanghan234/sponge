#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // discard bytes beyond current window

    if ( eof ) 
    {
        _eof_mark = data.size() + index;
        _eof_flag = true;
    }

    size_t cnt = index;
    while ( cnt < _expect && cnt - index < data.size() ) cnt++;
    while ( cnt - _expect < _capacity && cnt - index < data.size() )
    {
        while ( cnt - _expect >= _buffer.size() )
        {
            _buffer.push_back('0');
            _used.push_back(false);
        }
        _buffer[cnt-_expect] = data[cnt-index];
        if ( !_used[cnt-_expect] ) _unassembled++;
        _used[cnt-_expect] = true;
        cnt++;
    }

    std::string str;
    cnt = 0; 
    while ( cnt < _buffer.size() && cnt < _output.remaining_capacity() && _used[cnt] )
    {
        str += _buffer[cnt];
        cnt++;
    }

    if ( str.size() > 0 ) _output.write(str);
    
    cnt = 0;
    while ( cnt < str.size() )
    {
        _buffer.pop_front();
        _used.pop_front();
        _expect++;
        _unassembled--;
        cnt++;
    }

    if ( _eof_flag && _expect == _eof_mark )
        _output.end_input();
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled; }

bool StreamReassembler::empty() const { return _buffer.empty(); }
