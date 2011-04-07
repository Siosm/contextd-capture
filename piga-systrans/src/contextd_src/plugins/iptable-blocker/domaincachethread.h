#ifndef DOMAINCACHETHREAD_H
#define DOMAINCACHETHREAD_H

#include <QThread>
#include <QList>
#include <QUrl>
#include <QMap>

#include <QReadWriteLock>

class RuleList;

class DomainCacheThread : public QThread
{
	QReadWriteLock rwLock;

	RuleList* rlist;

	QString _appName;
	QUrl _url;
	QThread* cThread;

	QMap<QString, QList<QUrl> > domainCache;

	QUrl urlToCacheUrl(QUrl url);

	QList<QUrl> getURLsFromPage(const QString& page);

	void addURLList(QList<QUrl> urls, const QUrl& toUrl, const QString& appName);

	bool updateURLCache(const QString& appName, const QUrl& url);

public:
	explicit DomainCacheThread(RuleList* ruleList);

	QThread* currentThread();

	bool updateCache(const QString& appName, const QUrl& url);

protected:
	void run();
};

#endif // DOMAINCACHETHREAD_H
