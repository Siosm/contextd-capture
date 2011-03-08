#include<libnotify/notify.h>

NotifyNotification *notification;

int initNotify(const char* appName)
{
	notification=NULL;

	return notify_init(appName);
}

int sendNotification(const char* title, const char* message, int timeout, int priority, const char* img)
{
	if(notification==NULL)
		notification = notify_notification_new(title, message, img, NULL);
	else
		notify_notification_update (notification, title, message, img);

	notify_notification_set_timeout(notification, timeout);
	if(priority==0)
		notify_notification_set_urgency (notification, NOTIFY_URGENCY_LOW);
	else if(priority==1)
		notify_notification_set_urgency (notification, NOTIFY_URGENCY_NORMAL);
	else if(priority==2)
		notify_notification_set_urgency (notification, NOTIFY_URGENCY_CRITICAL);
	else
		notify_notification_set_urgency (notification, NOTIFY_URGENCY_NORMAL);

	GError *error = NULL;
	return notify_notification_show(notification, &error);
}

void closeNotify()
{
	GError *error = NULL;
	if(notification!=NULL)
		notify_notification_close (notification, &error);

	notify_uninit();
}

/*int main(int argc, char **argv)
{
	initNotify("mupuf_test");
	sendNotification("titre", "le message long !!!", 1500, 2, "");
	closeNotify();
}*/

/*int main(int argc, char **argv)
{
    // initialize gtk
    gtk_init(&argc,&argv);
    
    char name[40] = "Sample Notification";
    
    // initiate notify
    notify_init(name);
    
    // create a new notification
    NotifyNotification *example;
    example = notify_notification_new(name,"Checking it out",NULL,NULL);
    
    // create an icon for the notification
    GtkStatusIcon *icon = gtk_status_icon_new_from_stock (GTK_STOCK_YES);
    gtk_status_icon_set_visible(icon,TRUE);
    // attach that icon to the notification
    notify_notification_attach_to_status_icon (example,icon);
    
    // set the timeout of the notification to 3 secs
    notify_notification_set_timeout(example,3000);
    
    // set the category so as to tell what kind it is
    char category[30] = "Testing Notifications";
    notify_notification_set_category(example,category);
    
    // set the urgency level of the notification
    notify_notification_set_urgency (example,NOTIFY_URGENCY_CRITICAL);
    
    GError *error = NULL;
    notify_notification_show(example,&error);

}*/
