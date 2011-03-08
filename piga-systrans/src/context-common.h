#define SERVICE_NAME			"com.piga.contextd"
#define CONTEXTLISTENER_NAME		"com.piga.contextlistener"
#define LISTENER_PATH			"/"

#define SCOPE_LOCAL				"local"
#define SCOPE_GLOBAL			"global"

#define DBUS_SUCCESS			"OK"
#define DBUS_ERROR				"NOK"
#define DBUS_ERROR_NOT_REGISTERED	"NOT_REGISTERED"

#define CONTEXT_ACCEPT			"ACCEPT"
#define CONTEXT_REFUSE			"REFUSE"

//Parameters
	//Common
	#define CONTEXT_STATUS		"status" //For now, values can be either "connect" or "close"

	//Net
	#define CONTEXT_PROTOCOL		"protocol"
	#define CONTEXT_HOST		"host"
	#define CONTEXT_PORT		"port"
	#define CONTEXT_PATH		"path"

	//mail
	#define CONTEXT_FROM		"from"
	#define CONTEXT_TO			"to"
	#define CONTEXT_SESSION_TYPE	"session_type" //Can be either "unknown", "imap", "news", "smtp" or "pop3"

	//Files
	#define CONTEXT_FILE		"file"

	//Context
	#define CONTEXT_CONTEXT		"context"
