extern "C"
{
	int initNotify(const char* appName);

	//timeout in ms
	//priority : 0 = lowest, 1=normal, 2=critical
	int sendNotification(const char* title, const char* message, int timeout, int priority, const char* img);

	int closeNotify();
}
