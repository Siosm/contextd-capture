#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QtDBus>

#include "dbus-context.h"
#include "kernel-context.h"
#include "clientscleaner.h"
#include "misc.h"
#include "config-daemon.h"

#include "configuration.h"
#include "plugins/domainholder/domainholder.h"
#include "plugins/contextdpluginreloadevent.h"
#include "eventdispatcher.h"
#include "pigahandler.h"
#include "auditsec_lsm/syscall.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static void child_handler(int signum)
{
	QString pid_path=QString("/var/run/%1.pid").arg(DAEMON_NAME);

	switch(signum)
	{
		case SIGALRM:
			qDebug("Signal handler: ALARM");
			auditsec_register(0);
			exit(EXIT_FAILURE);
			break;
		case SIGUSR1:
			qDebug("Signal handler: USR1");
			auditsec_register(0);
			exit(EXIT_SUCCESS);
			break;
		case SIGCHLD:
			qDebug("Signal handler: CHLD");
			auditsec_register(0);
			exit(EXIT_FAILURE);
			break;
		case SIGKILL:
			qDebug("Signal handler: KILL");
			auditsec_register(0);
			exit(EXIT_FAILURE);
			break;
		case SIGTERM:
			qDebug("Signal handler: TERM");
			EventDispatcher::instance().sendNotification("Stopping the daemon");

			auditsec_register(0);
			remove(qPrintable(pid_path));

			exit(EXIT_SUCCESS);
			break;
		default:
			break;
	}

	//Reload the configuration
	if (signum == SIGUSR2){
		EventDispatcher::instance().sendNotification("Reloading the rules");
		Configuration::instance_nonconst().updateRules();

		ContextdPluginReloadEvent reloadEvent;
		//Send the reload Event
		EventDispatcher::instance().sendEvent(&reloadEvent);
	}


}

static void daemonize( const char *daemon_name, bool foreground )
{
	pid_t pid, sid; //, parent;
	int lfp = -1;

	//Already a daemon
	if ( getppid() == 1 ) return;

	//Create the lock file as the current user
	if ( daemon_name && daemon_name[0] ) {
		QString lock_path=QString("/var/lock/%1").arg(daemon_name);

		lfp = open(qPrintable(lock_path),O_RDWR|O_CREAT,0600);
		if ( lfp < 0 ) {
			qWarning( "Unable to open the lock file '%s', code=%i (%s)\nCheck your permissions\n", qPrintable(lock_path), errno, strerror(errno));
			exit(EXIT_FAILURE);
		}

		//Lock the descriptor
		if(::flock(lfp, LOCK_EX|LOCK_NB)!=0)
		{
			qWarning( "Unable to create the lock file '%s', code=%i (%s)\nThe daemon may already be running.\n", qPrintable(lock_path), errno, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	//Drop user if there is one, and we were run as root
	if ( getuid() == 0 || geteuid() == 0 ) {
		struct passwd *pw = getpwnam(RUN_AS_USER);
		if ( pw ) {
			qDebug("Contextd: setting user to %s\n", RUN_AS_USER);
			setuid( pw->pw_uid );
		}
	}

	if(!foreground)
	{
		//Trap signals that we expect to receive
		signal(SIGCHLD,child_handler);
		signal(SIGUSR1,child_handler);
		signal(SIGALRM,child_handler);
		signal(SIGINT,child_handler);
		signal(SIGTERM,child_handler);

		//Fork off the parent process
		pid = fork();
		if (pid < 0) {
			qWarning( "Unable to fork the daemon, code=%i (%s)\n", errno, strerror(errno));
			exit(EXIT_FAILURE);
		}
		//If we got a good PID, then we can exit the parent process.
		if (pid > 0)
		{
			sleep(1);
			exit(EXIT_SUCCESS);
		}

		//At this point we are executing as the child process
	}

	//Change the file mode mask
	umask(033);

	//Save the PID of the daemon into /var/run/
	QString pid_path=QString("/var/run/%1.pid").arg(daemon_name);
	FILE* fpid=fopen(qPrintable(pid_path), "w");
	if(fpid!=NULL)
	{
		QString pid=QString::number(getpid());
		qDebug("The daemon has the PID %s\n", qPrintable(pid));
		fwrite(qPrintable(pid), sizeof(char), pid.size()+1, fpid);
		fclose(fpid);
	}
	else
	{
		qWarning( "Unable to write the PID file to '%s', code %i (%3s)\n", qPrintable(pid_path), errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	signal(SIGCHLD,SIG_DFL); // A child process dies
	signal(SIGTSTP,SIG_IGN); // Various TTY signals
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP, SIG_IGN); // Ignore hangup signal
	signal(SIGTERM,child_handler); // Die on SIGTERM
	signal(SIGINT,child_handler);


	//Redirect standard files to /dev/null
	if(!foreground)
	{
		//Create a new SID for the child process
		sid = setsid();
		if (sid < 0) {
			qWarning( "Unable to create a new session, code %i (%s)\n", errno, strerror(errno));
			exit(EXIT_FAILURE);
		}

		/*Change the current working directory.  This prevents the current
		directory from being locked; hence not being able to remove it. */
		if ((chdir("/")) < 0) {
			qWarning( "Unable to change the current working directory to '%s', code %i (%s)\n", "/", errno, strerror(errno));
			exit(EXIT_FAILURE);
		}

		FILE* f=freopen( "/dev/null", "r", stdin);
		f=freopen( "/dev/null", "w", stdout);
		f=freopen( "/dev/null", "w", stderr);
	}

	//receive the reload rule signal
	signal(SIGUSR2, child_handler);
	signal(SIGTERM, child_handler);
}

int readPID(const char *daemon_name)
{
	//Get the PID of the daemon into /var/run/
	QString pid_path="/var/run/%1.pid";
	pid_path=pid_path.arg(daemon_name);

	QFile pid_file(pid_path);
	if(pid_file.open(QIODevice::ReadOnly))
	{
		QString line=pid_file.readLine();

		bool ok;
		int pid=line.toInt(&ok);

		if(ok)
			return pid;
		else
		{
			qWarning( "Invalid PID file '%s'\n", qPrintable(pid_path));
			return 0;
		}
	}
	else
	{
		qWarning( "Unable to read the PID file to '%s'\n", qPrintable(pid_path));
		return 0;
	}
}

void usage(const char* prog_path)
{
	printf("Usage :\n	%1$s: Load the daemon\n	%1$s --reload: Reload the rules\n	%1$s --stop: Stop the daemon\n	%1$s --foreground: Don't daemonize (useful for debugging)\n	%1$s --disable-piga: Don't ask PIGA-IPS to validate transitions\n", prog_path);
	exit(EXIT_FAILURE);
}

bool parsing(int argc, char **argv, bool& foreground, bool& piga)
{
	//Doing some parsing
	if(argc>1)
	{
		std::string cmd_reload="--reload";
		std::string cmd_stop="--stop";
		std::string cmd_foreground="--foreground";
		std::string cmd_disablepiga="--disable-piga";

		//read daemon's PID from file
		int pid=readPID(DAEMON_NAME);

		int i;
		for (i=1; i<argc; ++i)
		{
			if(argv[i]==cmd_reload)
			{
				if(pid>0)
					kill(pid, SIGUSR2);
				else
					qWarning( "Failed to reload the rules. The daemon is not currently running.\n");
				exit(0);
			}
			else if(argv[i]==cmd_stop)
			{
				if(pid>0)
					kill(pid, SIGTERM);
				else
					qWarning( "Failed to stop the daemon. The daemon is not currently running.\n");
				exit(0);
			}
			else if(argv[i]==cmd_foreground)
				foreground=true;
			else if(argv[i]==cmd_disablepiga)
				piga=false;
			else
				return false;
		}
	}

	return true;
}

int main(int argc, char **argv)
{
	//If we are not root, just exit
	if ( getuid() !=0 && geteuid() !=0)
	{
		qWarning( "Error : Only root can launch this daemon !\n");
		exit(EXIT_FAILURE);
	}

	//Init Qt
	QCoreApplication app(argc, argv);

	//Parse the command line
	bool foreground=false;
	bool piga=true;
	if(!parsing(argc, argv, foreground, piga))
		usage(argv[0]);

	//Daemonize
	daemonize(DAEMON_NAME, foreground);

	//Setup PIGA handler instance
	if (!piga)
		PigaHandler::instance().disable();

	//Start DBUS & Kernel LSM listening
	DBusContext dbus;
	KernelContext kernel;
	ClientsCleaner cc(kernel.getClients());

	//Log the startup
	EventDispatcher::instance().sendNotification("Contextd started");

	//Set the current context to the default one
	DomainHolder::instance().setDefaultDomain(Configuration::instance().defaultDomain());
	DomainHolder::resetToDefaultDomain();

	//Sets other loops to wait for app.exec to start (Kernel & Cleaner)
	QTimer * cleanerTimer = new QTimer();
	cleanerTimer->setInterval(120000); // Check every 2 minutes = 120 000 milliseconds
	QObject::connect(cleanerTimer, SIGNAL(timeout()), cleanerTimer, SLOT(start()));
	QObject::connect(cleanerTimer, SIGNAL(timeout()), &cc, SLOT(start()));

	QTimer::singleShot(0, cleanerTimer, SLOT(start()));
	QTimer::singleShot(0, &kernel, SLOT(start()));

	//Start the event loop
	app.exec();

	//Stop Kernel & Cleaner loop
	kernel.stop();
	cleanerTimer->stop();
	cc.terminate();

	// Finish up
	EventDispatcher::instance().sendNotification("Contextd terminated correctly\n\n");

	return 0;
}
