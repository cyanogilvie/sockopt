
set socket	[socket localhost 1234]

sockopt::setsockopt $socket SOL_SOCKET	SO_KEEPALIVE	1
sockopt::setsockopt $socket SOL_TCP		TCP_KEEPCNT		20
sockopt::setsockopt $socket SOL_TCP		TCP_KEEPIDLE	180
sockopt::setsockopt $socket SOL_TCP		TCP_KEEPINTVL	60



