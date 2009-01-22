package require sockopt

set listen	[socket -server [list apply {
	{socket cl_ip cl_port} {
		puts "got connection from $cl_ip:$cl_port"

		sockopt::setsockopt $socket SOL_SOCKET	SO_KEEPALIVE	1
		sockopt::setsockopt $socket SOL_TCP		TCP_KEEPIDLE	20
		sockopt::setsockopt $socket SOL_TCP		TCP_KEEPCNT		2
		sockopt::setsockopt $socket SOL_TCP		TCP_KEEPINTVL	5
		sockopt::setsockopt $socket IPPROTO_TCP	TCP_NODELAY		1

		chan event $socket readable [list apply {
			{socket ip port} {
				set dat	[read $socket]
				if {[chan eof $socket]} {
					puts "client disconnected: $ip:$port"
					close $socket
					return
				}
			}
		} $socket $cl_ip $cl_port]
	}
}] 1234]

vwait ::forever
