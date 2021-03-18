#include "tcp_receiver.hh"
#include <optional>

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader head = seg.header();
    const string body = seg.payload().copy();

    // if _status is wait and syn is not set, return
    if ( ( _status == STANDBY ) && !head.syn ) return;
    // if _status is syn_recv and syn is set, ignore, return
    if ( ( _status == SYN_RECV ) && head.syn ) return;

    if ( head.syn && head.fin )
    {
        if ( !_isn_set ) 
        {
            _isn_set = true;
            _isn = head.seqno;
            _ack++;
        }
        WrappingInt32 seqno = head.seqno + 1;
        uint64_t abs_seq = unwrap(seqno,_isn,_checkpoint);
        _checkpoint = abs_seq;
        _reassembler.push_substring(body,abs_seq-1,true);
        _ack = _reassembler.expect() + 1;
        _status = FIN_RECV;
    }
    else if ( head.syn )
    {
        if ( !_isn_set ) 
        {
            _isn_set = true;
            _isn = head.seqno;
            _ack++;
        }
        _status = SYN_RECV;
        WrappingInt32 seqno = head.seqno + 1;
        uint64_t abs_seq = unwrap(seqno,_isn,_checkpoint);
        _checkpoint = abs_seq;
        _reassembler.push_substring(body,abs_seq-1,false);
        _ack = _reassembler.expect() + 1;
    }
    else
    {
        WrappingInt32 seqno = head.seqno;
        uint64_t abs_seq = unwrap(seqno,_isn,_checkpoint);
        _checkpoint = abs_seq;
        if ( head.fin )
            _reassembler.push_substring(body,abs_seq-1,true);
        else
            _reassembler.push_substring(body,abs_seq-1,false);
        _ack = _reassembler.expect() + 1;
        if ( head.fin )
        {
            _status = FIN_RECV;
        }
    }

    if ( _status == FIN_RECV && stream_out().input_ended() )
        _ack++;
}

std::optional<WrappingInt32> TCPReceiver::ackno() const {
    if ( _isn_set )
        return wrap(_ack,_isn);
    else
        return {};
}

size_t TCPReceiver::window_size() const { return stream_out().remaining_capacity(); }