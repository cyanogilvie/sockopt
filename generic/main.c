#include <tclstuff.h>
#include <string.h>
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
	int					s, optval, res;
	Tcl_Channel			chan;
	Tcl_ChannelType*	chantype;

	static CONST char* levels[] = {
		"SOL_SOCKET",
		"SOL_TCP",
		"IPPROTO_TCP",
		(char *)NULL
	};
	int level_map[] = {
		SOL_SOCKET,
		SOL_TCP,
		IPPROTO_TCP
	};
	int level_index;

	static CONST char* optnames[] = {
		"SO_KEEPALIVE",
		"TCP_KEEPCNT",
		"TCP_KEEPIDLE",
		"TCP_KEEPINTVL",
		"TCP_NODELAY",
		(char *)NULL
	};
	int optname_map[] = {
		SO_KEEPALIVE,
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

	if (Tcl_GetChannelHandle(chan, TCL_WRITABLE, (ClientData*)&s) != TCL_OK)
		THROW_ERROR("Could not retrieve OS socket handle");

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[2], levels, "level",
				TCL_EXACT, &level_index));

	TEST_OK(Tcl_GetIndexFromObj(interp, objv[3], optnames, "optname",
				TCL_EXACT, &optname_index));

	TEST_OK(Tcl_GetIntFromObj(interp, objv[4], &optval));

	fprintf(stderr, "setsockopt(%d, %d, %d, %d, %d);\n",
			s, level_map[level_index], optname_map[optname_index],
			optval, sizeof(int));
	res = setsockopt(s, level_map[level_index], optname_map[optname_index],
			(const void*)&optval, sizeof(int));

	if (res == 0) {
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
	int					s, optval, res;
	socklen_t			optval_len;
	Tcl_Channel			chan;
	Tcl_ChannelType*	chantype;

	CHECK_ARGS(1, "socket_handle");

	chan = Tcl_GetChannel(interp, Tcl_GetString(objv[1]), NULL);
	if (chan == NULL)
		THROW_ERROR("No such channel: ", Tcl_GetString(objv[1]));

	chantype = Tcl_GetChannelType(chan);
	if (strcmp(Tcl_ChannelName(chantype), "tcp") != 0)
		THROW_ERROR("Not a TCP channel");

	if (Tcl_GetChannelHandle(chan, TCL_WRITABLE, (ClientData*)&s) != TCL_OK)
		THROW_ERROR("Could not retrieve OS socket handle");

	res = getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void*)&optval, &optval_len);
	fprintf(stderr, "%d = getsockopt(s, SOL_SOCKET, SO_KEEPALIVE): %d\n",
			res, optval);

	res = getsockopt(s, SOL_TCP, TCP_KEEPIDLE, (void*)&optval, &optval_len);
	fprintf(stderr, "%d = getsockopt(s, SOL_TCP, TCP_KEEPIDLE): %d\n",
			res, optval);

	res = getsockopt(s, SOL_TCP, TCP_KEEPCNT, (void*)&optval, &optval_len);
	fprintf(stderr, "%d = getsockopt(s, SOL_TCP, TCP_KEEPCNT): %d\n",
			res, optval);

	res = getsockopt(s, SOL_TCP, TCP_KEEPINTVL, (void*)&optval, &optval_len);
	fprintf(stderr, "%d = getsockopt(s, SOL_TCP, TCP_KEEPINTVL): %d\n",
			res, optval);

	res = getsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void*)&optval, &optval_len);
	fprintf(stderr, "%d = getsockopt(s, SOL_TCP, TCP_NODELAY): %d\n",
			res, optval);

	return TCL_OK;
}

//>>>
int Sockopt_Init(Tcl_Interp *interp) //<<<
{
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL)
		return TCL_ERROR;

	NEW_CMD("sockopt::setsockopt", glue_setsockopt);
	NEW_CMD("sockopt::dumpsockopt", glue_dumpsockopt);

	TEST_OK(Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION));

	return TCL_OK;
}

//>>>

// vim: foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
