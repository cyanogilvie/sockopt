package require sockopt

set socket	[socket localhost 1234]

sockopt::setsockopt $socket SOL_SOCKET	SO_KEEPALIVE	1
sockopt::setsockopt $socket SOL_TCP		TCP_KEEPIDLE	20
sockopt::setsockopt $socket SOL_TCP		TCP_KEEPCNT		2
sockopt::setsockopt $socket SOL_TCP		TCP_KEEPINTVL	5
sockopt::setsockopt $socket IPPROTO_TCP	TCP_NODELAY		1

chan event $socket readable [list apply {
	{socket} {
		set dat	[read $socket]
		if {[chan eof $socket]} {
			puts "socket closed"
			close $socket
			return
		}
	}
} $socket]

sockopt::dumpsockopt $socket


vwait ::forever
