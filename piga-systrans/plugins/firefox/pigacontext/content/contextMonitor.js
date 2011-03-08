const NOTIFY_STATE_DOCUMENT =
  Components.interfaces.nsIWebProgress.NOTIFY_STATE_DOCUMENT;
const STATE_IS_DOCUMENT =
  Components.interfaces.nsIWebProgressListener.STATE_IS_DOCUMENT;

const NOTIFY_STATE_WINDOW =
  Components.interfaces.nsIWebProgress.NOTIFY_STATE_WINDOW;
const STATE_IS_WINDOW =
  Components.interfaces.nsIWebProgress.STATE_IS_WINDOW;

const NOTIFY_STATE_ALL =
  Components.interfaces.nsIWebProgress.NOTIFY_STATE_ALL;
const NOTIFY_PROGRESS =
  Components.interfaces.nsIWebProgress.NOTIFY_PROGRESS;

const STATE_START =
  Components.interfaces.nsIWebProgressListener.STATE_START;

const ERROR_PAGE="chrome://pigacontext/content/access_forbidden.xul";

//global variables
var classPIGA;

function clearCache()
{
    var cacheClass = Components.classes["@mozilla.org/network/cache-service;1"];
    var cacheService = cacheClass.getService(Components.interfaces.nsICacheService);
    var alertService = Components.classes["@mozilla.org/alerts-service;1"].getService(Components.interfaces.nsIAlertsService);
    alertService.showAlertNotification("", "Debug: Clear cache", "Clear cache !!", false, "", null);

    try
    {
        cacheService.evictEntries(Components.interfaces.nsICache.STORE_ON_DISK);
    }
    catch(exception)
    {
        //alertService.showAlertNotification("", "Debug: Failed to clear cache", exception, false, "", null);
    }

    try
    {
        cacheService.evictEntries(Components.interfaces.nsICache.STORE_IN_MEMORY);
    }
    catch(exception)
    {
        alertService.showAlertNotification("", "Debug: Failed to clear cache", exception, false, "", null);
    }
}

function onContextChanged()
{
	var oldContext=document.getElementById("panel_pigacontext").label;
	
	var ccontext=classPIGA.currentGlobalContext();
	document.getElementById("panel_pigacontext").label = "Current Context = "+ccontext+"\n";
	
	if(oldContext!=document.getElementById("panel_pigacontext").label)
		clearCache();
}

function updateContext(url)
{
	if(url!=ERROR_PAGE)
	{
		//Update the current Context
		currentUrl=url;

		//Add the wanted URL in the tab's attribute so as we can restore it later
		var x = document.createElement("pigaWantedURL");
		var selectedTabIndex=getBrowser().mTabContainer.selectedIndex;
		var selectedTab=document.getElementById("content").mTabs[selectedTabIndex];
		selectedTab.appendChild(x);
		selectedTab.pigaWantedURL=url;
		
		//Inform Contextd
		var ret=classPIGA.urlChanged(url);
		dump("New Url = "+url+"\n");
		
		dump("urlChanged returnd = '"+ret+"'\n");

		//Update the status bar
		if(ret=="accepted")
			onContextChanged();
		else
			getBrowser().loadURI(ERROR_PAGE);
	}
}

function checkTabsPermissions(new_context)
{
	var content = document.getElementById("content");
	var tab_count=content.mTabs.length;
	var i;
	for(i=0;i<tab_count;i++)
	{
		try
		{
			var tab_uri=content.mTabs[i].pigaWantedURL;
			dump("requiredContext: ");
			var required_context=classPIGA.requiredContext(tab_uri);
			dump("OK\n");

			if(required_context!=new_context)
			{
				dump("	Tab"+i+": URL="+tab_uri+"; "+required_context+"!="+new_context+", so, we will redirect it !\n");
				content.mTabs[i].linkedBrowser.loadURI(ERROR_PAGE);
			}
			else
				content.mTabs[i].linkedBrowser.loadURI(tab_uri);
		}
		catch(err)
		{
			//On errror, hide the page
			content.mTabs[i].linkedBrowser.loadURI(ERROR_PAGE);
		}
	}
}

var timer_currentContext;
function onTimer()
{
	var new_context=classPIGA.currentGlobalContext();
	if(new_context!=timer_currentContext)
	{
		dump("Context changed from "+timer_currentContext+" to "+new_context+" !\n");
		checkTabsPermissions(new_context);
		timer_currentContext=new_context;
	}
}

function onUrlChanged()
{
	updateContext(gURLBar.value);
}

function onTabChange()
{
	updateContext(gURLBar.value);
}

function getObserverService()
{
	return Components.classes["@mozilla.org/observer-service;1"].getService(Components.interfaces.nsIObserverService);
} 

function registerMyListener()
{
	//Init global variables
	classPIGA = Components.classes["@sds-project/PIGA;1"].createInstance(Components.interfaces.PIGAContext);		

	window.getBrowser().addProgressListener(myListener , NOTIFY_STATE_ALL);
	
	//getBrowser().mTabContainer.addEventListener("select", onTabChange,false);
	
	//Changing the url in the urlbar
	gURLBar.addEventListener("ValueChange", onUrlChanged, true);

	//Listen to contexts changing
	var observeService = getObserverService();
	observeService.addObserver(myListener, "globalContextChanged", false);

	//Show current context
	onContextChanged();
	
	//Load the timer
	setInterval("onTimer()", 200);
}

function unregisterMyListener()
{
	window.getBrowser().removeProgressListener(myListener);
}

window.addEventListener("load",registerMyListener,false);
window.addEventListener("unload",unregisterMyListener,false);

var myListener =
{
	onStateChange:function(aProgress,aRequest,aFlag,aStatus)
	{
		if(aFlag & (STATE_START))
		{
			updateContext(aRequest.URI.spec);
		}
	},
	onLocationChange:function(a,b,c){},
	onProgressChange:function(aBrowser,webProgress,request,d,e,f)
	{

	},
	onStatusChange:function(a,b,c,d){},
	onSecurityChange:function(a,b,c){},

	/*XXX
	This is not nsIWebProgressListenr method,
	just killing a error in tabbrowser.xml
	Maybe a bug.
	*/
	onLinkIconAvailable:function(a){},
	
	//On context change
	observe: function im_observer(subject, topic, someData)
	{
		var new_context=someData;
		dump("Context changed: "+new_context+"\n");

		checkTabsPermissions(new_context);
	}
}
