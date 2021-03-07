#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {
    buffer.resize(_capacity);
    used.resize(_capacity);
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // discard bytes beyond current window
    size_t ind = index;
    while ( buffered_bytes < _capacity && ind - index < data.size() )
    {
        if ( ind < expect )
        {
            ind++;
            continue;
        }
        else if ( ind - expect >= _capacity )
            break;
        buffer[(ind-expect+next)%_capacity] = data[ind-index];
        if ( !used[(ind-expect+next)%_capacity] )
        {
            used[(ind-expect+next)%_capacity] = true;
            buffered_bytes++;
        }
        ind++;
    }
    std::string assembled;
    size_t cnt = 0;
    while ( used[next] && cnt < _output.remaining_capacity() )
    {
        assembled += buffer[next];
        used[next++] = false;
        buffered_bytes--;
        next %= _capacity;
        expect++;
        cnt++;
    }

    if ( assembled.size() > 0 )
        _output.write(assembled);
    if ( eof )
    {
        if ( expect >= index && expect-index == data.size() )
            _output.end_input();
        else
        {
            eof_mark = index + data.size();
            eof_set = true;
        }
    }
    else if ( eof_set && expect == eof_mark )
        _output.end_input();
}

size_t StreamReassembler::unassembled_bytes() const { return buffered_bytes; }

bool StreamReassembler::empty() const { return unassembled_bytes() == 0; }
