#include "domaincachethread.h"

#include <QSet>
#include "synchttp.h"
#include "../domainholder/domainholder.h"
#include "../../../context-common.h"
#include "program.h"
#include "configuration.h"
#include "rulelist.h"

QUrl DomainCacheThread::urlToCacheUrl(QUrl url)
{
	QUrl ret;

	ret.setScheme(url.scheme());
	ret.setHost(url.host());
	ret.setPort(url.port());
	ret.setPath("/");

	return ret;
}

DomainCacheThread::DomainCacheThread(RuleList* ruleList) : rlist(ruleList)
{
}

QThread* DomainCacheThread::currentThread()
{
	return cThread;
}

QList<QUrl> DomainCacheThread::getURLsFromPage(const QString& page)
{
	QSet<QString> s_urls;

	// Src urls
	QRegExp rx_src("<.*src=\"(https?://[^\"]*)\".*>");
	rx_src.setMinimal(true);
	int pos = 0;
	while ((pos = rx_src.indexIn(page, pos)) != -1) {
		s_urls.insert(rx_src.cap(1));
		pos += rx_src.matchedLength();
	}

	QRegExp rx_links("<link .*href=\"(https?://[^\"]*)\".*>");
	rx_links.setMinimal(true);
	pos = 0;
	while ((pos = rx_links.indexIn(page, pos)) != -1) {
		s_urls.insert(rx_links.cap(1));
		pos += rx_links.matchedLength();
	}

	QList<QUrl> ret;
	QList<QString> url_l=s_urls.toList();
	for(int i=0; i<url_l.size(); i++)
		ret.push_back(url_l[i]);

	return ret;
}

void DomainCacheThread::addURLList(QList<QUrl> urls, const QUrl& toUrl, const QString& appName)
{
	// Try to add the urls
	for(int i=0; i<urls.size(); i++)
	{
		QUrl tmpUrl(urls[i]);
		if(tmpUrl.port()==-1)
			tmpUrl.setPort(tmpUrl.scheme().toLower()=="http"?80:443);

		//Fill a state
		QMap<QString, QString> state;
		state[CONTEXT_URL]=tmpUrl.toString();
		state[CONTEXT_PROTOCOL]=tmpUrl.scheme();
		state[CONTEXT_HOST]=tmpUrl.host();
		state[CONTEXT_PORT]=QString::number(tmpUrl.port());
		state[CONTEXT_PATH]=tmpUrl.path();

		//Check if this is allowed
		Program program=Configuration::instance().getProgramByName(appName);
		Domain cDomain=DomainHolder::currentDomain(); //Domain("ecommerce", "", "");
		if(Configuration::instance().getFirstMatchingTransition(program, cDomain, state).isValid())
		{
			domainCache[toUrl.host()].push_back(urlToCacheUrl(tmpUrl));
			rlist->addDomain(_appName, tmpUrl.host(), tmpUrl.port());
		}
	}
}

#include <QTime>
bool DomainCacheThread::updateURLCache(const QString& appName, const QUrl& url)
{
	QTime time=QTime::currentTime();

	// Get request
	SyncHTTP shttp(url.host());
	QHttp::ConnectionMode mode = url.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
	shttp.setHost(url.host(), mode, url.port() == -1 ? 0 : url.port());

	// Issue the get request
	QBuffer getOutput;
	if(!shttp.syncGet(url.path(), &getOutput))
		return false;

	// Clear the current cache
	QUrl urlCache=urlToCacheUrl(url);
	domainCache[urlCache.host()].clear();

	// Get a list of urls
	QList<QUrl> urls=getURLsFromPage(getOutput.data());

	// Add the urls
	addURLList(urls, url, appName);

	qDebug("updateURLCache: Cache updated in %imsfor '%s' with %i adresses", time.msecsTo(QTime::currentTime()), qPrintable(url.toString()), urls.size());

	return true;
}

bool DomainCacheThread::updateCache(const QString& appName, const QUrl& url)
{
	// Get the domain list
	wait();
	QList<QUrl> listDomain=domainCache[url.host()];

	// If we have entries for the url
	if(listDomain.size()>0)
	{
		// Add them now to IPTables
		addURLList(listDomain, url, appName);
	}


	// Let's update the cache
	this->_appName=appName;
	this->_url=url;
	start();

	// Wait a bit if we previously didn't find any entry
	if(listDomain.size()==0)
		wait(1000);

	return true;
}

void DomainCacheThread::run()
{
	cThread=QThread::currentThread();
	updateURLCache(_appName, _url);
	cThread=NULL;
}
