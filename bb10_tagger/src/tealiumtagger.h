#ifndef TEALIUMTAGGER_H_
#define TEALIUMTAGGER_H_

#include <QMap>
#include <QVariant>
#include <QLinkedList>
#include <bb/cascades/CustomControl>
#include <bb/cascades/WebLoadRequest>
#include <bb/cascades/Page>
#include <bb/cascades/Application>
#include <bb/ProcessState>

typedef QMap<QString, QString> Map;

class TealiumTagger: public bb::cascades::CustomControl
{
Q_OBJECT

public:
	TealiumTagger(bb::cascades::Application* app, QString account, QString profile, QString environment);
	TealiumTagger(bb::cascades::Application* app, QString account, QString profile, QString environment, Map variables);
	void setVariables(Map variables);
	void setVariable(QString name, QString value);
	void trackItemClicked(QString itemName);
	void trackItemClicked(QString itemName, Map variables);
	void trackScreenViewed(QString viewName);
	void trackScreenViewed(QString viewName, Map variables);
	void trackCustomEvent(QString eventName, Map variables);

private:
    bb::cascades::WebView *webView;
    bb::cascades::Application *app;
    QString configAccount;
    QString configProfile;
    QString configEnvironment;
    Map variablesBase;
    Map variablesQueued;
    QLinkedList<QString> requestQueue;
	bool statusAwake;
	bool initialWebView;
	int initialWebViewState;
	bool connectionCurrent;
	bool sendingRequestItems;

	void init(bb::cascades::Application *app, QString account, QString profile, QString environment, Map variables);
	void initializeWebView();
    void sendRequestQueue();
	void setDeviceInfo();
    bool connectionTest();
    void sleep();
    void wakeUp();
    void saveQueueToDisk();
    void loadQueueFromDisk();
    void logger(QString msg, int level);

private Q_SLOTS:
	void onLoadingChanged(bb::cascades::WebLoadRequest *loadRequest);
	void initializeWebViewRetry();
    void sendRequestItem();
    void onAwake();
    void onAsleep();
    void onAboutToQuit();
    void connectionChange();
};

#endif
