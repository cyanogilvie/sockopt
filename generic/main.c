#include "tclstuff.h"
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

static int glue_setsockopt(cdata, interp, objc, objv) //<<<
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj* CONST	objv[];
{
	ClientData				st;
	int						s, optval, res, tmpoptval;
	Tcl_Channel				chan;
	const Tcl_ChannelType*	chantype;
	socklen_t				optlen = sizeof(optval);

	static CONST char* levels[] = {
		"SOL_SOCKET",
		"SOL_TCP",
		"IPPROTO_TCP",
		(char *)NULL
	};
	int level_map[] = {
		SOL_SOCKET,
		SOL_TCP,
#ifdef IPPROTO_TCP
		IPPROTO_TCP
#else
		SOL_TCP
#endif
	};
	int level_index;

	static CONST char* optnames[] = {
		"SO_KEEPALIVE",
		"SO_REUSEADDR",
		"TCP_KEEPCNT",
		"TCP_KEEPIDLE",
		"TCP_KEEPINTVL",
		"TCP_NODELAY",
		(char *)NULL
	};
	int optname_map[] = {
		SO_KEEPALIVE,
		SO_REUSEADDR,
		TCP_KEEPCNT,
		TCP_KEEPIDLE,
		TCP_KEEPINTVL,
		TCP_NODELAY
	};
	int optname_index;

	CHECK_ARGS(4, "socket_handle level optname optval");

	chan = Tcl_GetChannel(interp, Tcl_GetString(objv[1]), NULL);
	if (chan == NULL)
		THROW_ERROR("No such channel: ", Tcl_GetString(objv[1]));

	chantype = Tcl_GetChannelType(chan);
	if (strcmp(Tcl_ChannelName(chantype), "tcp") != 0)
		THROW_ERROR("Not a TCP channel");

	if (Tcl_GetChannelHandle(chan, TCL_WRITABLE, &st) != TCL_OK)
		THROW_ERROR("Could not retrieve OS socket handle");
	s = (ptrdiff_t)st;

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[2], levels, "level",
				TCL_EXACT, &level_index));

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[3], optnames, "optname",
				TCL_EXACT, &optname_index));

	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &optval));

	getsockopt(s, level_map[level_index], optname_map[optname_index],
			&tmpoptval, &optlen);

	res = setsockopt(s, level_map[level_index], optname_map[optname_index],
			(const void*)&optval, sizeof(int));

	getsockopt(s, level_map[level_index], optname_map[optname_index],
			&tmpoptval, &optlen);

	if (res == 0) {
		return TCL_OK;
	} else {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(Tcl_ErrnoMsg(Tcl_GetErrno()), -1));
		return TCL_ERROR;
	}
}

//>>>
static int glue_getsockopt(cdata, interp, objc, objv) //<<<
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj* CONST	objv[];
{
	ClientData				st;
	int						s, optval, res;
	int						tmpoptval;
	Tcl_Channel				chan;
	const Tcl_ChannelType*	chantype;
	socklen_t				optlen = sizeof(optval);
	Tcl_WideInt				staging_optval;

	static CONST char* levels[] = {
		"SOL_SOCKET",
		"SOL_TCP",
		"IPPROTO_TCP",
		(char *)NULL
	};
	int level_map[] = {
		SOL_SOCKET,
		SOL_TCP,
#ifdef IPPROTO_TCP
		IPPROTO_TCP
#else
		SOL_TCP
#endif
	};
	int level_index;

	static CONST char* optnames[] = {
		"SO_KEEPALIVE",
		"SO_REUSEADDR",
		"TCP_KEEPCNT",
		"TCP_KEEPIDLE",
		"TCP_KEEPINTVL",
		"TCP_NODELAY",
		(char *)NULL
	};
	int optname_map[] = {
		SO_KEEPALIVE,
		SO_REUSEADDR,
		TCP_KEEPCNT,
		TCP_KEEPIDLE,
		TCP_KEEPINTVL,
		TCP_NODELAY
	};
	int optname_index;

	CHECK_ARGS(3, "socket_handle level optname");

	chan = Tcl_GetChannel(interp, Tcl_GetString(objv[1]), NULL);
	if (chan == NULL)
		THROW_ERROR("No such channel: ", Tcl_GetString(objv[1]));

	chantype = Tcl_GetChannelType(chan);
	if (strcmp(Tcl_ChannelName(chantype), "tcp") != 0)
		THROW_ERROR("Not a TCP channel");

	if (Tcl_GetChannelHandle(chan, TCL_WRITABLE, &st) != TCL_OK)
		THROW_ERROR("Could not retrieve OS socket handle");
	s = (ptrdiff_t)st;

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[2], levels, "level",
				TCL_EXACT, &level_index));

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[3], optnames, "optname",
				TCL_EXACT, &optname_index));

	res = getsockopt(s, level_map[level_index], optname_map[optname_index],
			&optval, &optlen);

	getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &tmpoptval, &optlen);

	if (res == 0) {
		staging_optval = optval;
		Tcl_SetObjResult(interp, Tcl_NewWideIntObj(staging_optval));
		return TCL_OK;
	} else {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(Tcl_ErrnoMsg(Tcl_GetErrno()), -1));
		return TCL_ERROR;
	}
}

//>>>
static int glue_dumpsockopt(cdata, interp, objc, objv) //<<<
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj* CONST	objv[];
{
	ClientData				st;
	int						s, optval, res;
	Tcl_Channel				chan;
	const Tcl_ChannelType*	chantype;
	socklen_t				optlen = sizeof(optval);

	CHECK_ARGS(1, "socket_handle");

	chan = Tcl_GetChannel(interp, Tcl_GetString(objv[1]), NULL);
	if (chan == NULL)
		THROW_ERROR("No such channel: ", Tcl_GetString(objv[1]));

	chantype = Tcl_GetChannelType(chan);
	if (strcmp(Tcl_ChannelName(chantype), "tcp") != 0)
		THROW_ERROR("Not a TCP channel");

	if (Tcl_GetChannelHandle(chan, TCL_WRITABLE, &st) != TCL_OK)
		THROW_ERROR("Could not retrieve OS socket handle");
	s = (ptrdiff_t)st;
	fprintf(stderr, "dumpsockopt chan is \"%s\" s is %d\n", Tcl_GetString(objv[1]), s);

	res = getsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, &optlen);
	fprintf(stderr, "--- %d = getsockopt(%d, SOL_SOCKET, SO_REUSEADDR): %d\n",
			res, s, optval);

	res = getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen);
	fprintf(stderr, "--- %d = getsockopt(%d, SOL_SOCKET, SO_KEEPALIVE): %d\n",
			res, s, optval);

	res = getsockopt(s, SOL_TCP, TCP_KEEPIDLE, &optval, &optlen);
	fprintf(stderr, "--- %d = getsockopt(%d, SOL_TCP, TCP_KEEPIDLE): %d\n",
			res, s, optval);

	res = getsockopt(s, SOL_TCP, TCP_KEEPCNT, &optval, &optlen);
	fprintf(stderr, "--- %d = getsockopt(%d, SOL_TCP, TCP_KEEPCNT): %d\n",
			res, s, optval);

	res = getsockopt(s, SOL_TCP, TCP_KEEPINTVL, &optval, &optlen);
	fprintf(stderr, "--- %d = getsockopt(%d, SOL_TCP, TCP_KEEPINTVL): %d\n",
			res, s, optval);

	res = getsockopt(s, IPPROTO_TCP, TCP_NODELAY, &optval, &optlen);
	fprintf(stderr, "--- %d = getsockopt(%d, IPPROTO_TCP, TCP_NODELAY): %d\n",
			res, s, optval);

	return TCL_OK;
}

//>>>
int Sockopt_Init(Tcl_Interp *interp) //<<<
{
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL)
		return TCL_ERROR;

	NEW_CMD("sockopt::setsockopt", glue_setsockopt);
	NEW_CMD("sockopt::getsockopt", glue_getsockopt);
	NEW_CMD("sockopt::dumpsockopt", glue_dumpsockopt);

	TEST_OK(Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION));

	return TCL_OK;
}

//>>>

// vim: foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
