#include <tclstuff.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

static int glue_setsockopt(cdata, interp, objc, objv) //<<
	ClientData		cdata;
	Tcl_Interp*		interp;
	int				objc;
	Tcl_Obj* CONST	objv[];
{
	int					s, level, optname, optval, res;
	Tcl_Channel			chan;
	Tcl_ChannelType*	chantype;

	static CONST char* levels[] = {
		"SOL_SOCKET",
		"SOL_TCP",
		(char *)NULL
	};
	int* level_map[] = {
		SOL_SOCKET,
		SOL_TCP
	};
	int level_index;

	static CONST char* optnames[] = {
		"SO_KEEPALIVE",
		"TCP_KEEPCNT",
		"TCP_KEEPIDLE",
		"TCP_KEEPINTVL",
		(char *)NULL
	};
	int* optname_map[] = {
		SO_KEEPALIVE,
		TCP_KEEPCNT,
		TCP_KEEPIDLE,
		TCP_KEEPINTVL
	};
	int optname_index;

	CHECK_ARGS(4, "socket_handle level optname optval");

	chan = Tcl_GetChannel(interp, objv[1], NULL);
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

	res = setsockopt(s, level, optname, (const void*)optval, sizeof(int));

	if (res == 0) {
		return TCL_OK;
	} else {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(Tcl_ErrnoMsg(Tcl_Geterrno()), -1));
		return TCL_ERROR;
	}
}

//>>>

int Sockopt_Init(Tcl_Interp *interp) //<<<
{
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL)
		return TCL_ERROR;

	NEW_CMD("sockopt::setsockopt", glue_setsockopt);

	return TCL_OK;
}

//>>>

// vim: ft=tcl foldmethod=marker foldmarker=<<<,>>> ts=4 shiftwidth=4
