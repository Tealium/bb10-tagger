#include "tealiumtagger.h"
#include <QMap>
#include <QVariant>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <bb/ProcessState>
#include <bb/cascades/WebView>
#include <bb/cascades/WebLoadRequest>
#include <bb/cascades/Page>
#include <bb/cascades/Application>
#include <bb/platform/PlatformInfo>
#include <bb/ApplicationInfo>
#include <bb/PackageInfo>
#include <bb/data/JsonDataAccess>
#include <bps/bps.h>
#include <bps/netstatus.h>
#include <sstream>
#include <cstddef>

#define TAG "TEALIUM BB10 TAGGER"
#define CDN_ROOT "http://tags.tiqcdn.com/utag/"
#define LOGGER_LEVEL 0 // 0 - no messages, 1 - information, 2 - troubleshooting, 3 - everything
#define SEND_TIME_MIN 100
#define SEND_TIME_VAR 100
#define SEND_QUEUE_FILE "data/cache/tealium_request_queue.json"

TealiumTagger::TealiumTagger(bb::cascades::Application* app, QString account, QString profile, QString environment){
	Map variables = Map();
	init(app, account, profile, environment, variables);
}

TealiumTagger::TealiumTagger(bb::cascades::Application* app, QString account, QString profile, QString environment, Map variables){
	init(app, account, profile, environment, variables);
}

void TealiumTagger::init(bb::cascades::Application* app, QString account, QString profile, QString environment, Map variables){
	this->app = app;
	this->configAccount = account;
	this->configProfile = profile;
	this->configEnvironment = environment;
	this->webView = new bb::cascades::WebView();

	loadQueueFromDisk();

    //setup event handlers
	QObject::connect(this->app, SIGNAL(awake()), this, SLOT(onAwake()));
	QObject::connect(this->app, SIGNAL(asleep()), this, SLOT(onAsleep()));
	QObject::connect(this->app, SIGNAL(aboutToQuit()), this, SLOT(onAboutToQuit()));
    QObject::connect(this->app, SIGNAL(connectionChange()), this, SLOT(onConnectionChange()));
	QObject::connect(this->webView, SIGNAL(loadingChanged(bb::cascades::WebLoadRequest *)), this, SLOT(onLoadingChanged(bb::cascades::WebLoadRequest *)));

	//set data layer
	setVariables(variables);
	setDeviceInfo();
	bps_initialize(); //call to initialize the network connectivity engine

	//set initialization flags
	initialWebViewState = -1;
	statusAwake = true;
	sendingRequestItems = false;
	initializeWebView();
	logger("INIT COMPLETE", 1);
}

void TealiumTagger::setDeviceInfo(){
	bb::platform::PlatformInfo platformInfo;
	variablesBase.insert("os_version", platformInfo.osVersion());


	bb::ApplicationInfo applicationInfo;
	variablesBase.insert("app_name", applicationInfo.title());
	variablesBase.insert("app_version", applicationInfo.version());

	bb::PackageInfo packageInfo;
	variablesBase.insert("pkg_name", packageInfo.name());
	variablesBase.insert("pkg_version", packageInfo.version());
	variablesBase.insert("pkg_author", packageInfo.author());
	variablesBase.insert("platform", "bb10");

	// composite data sources
	variablesBase.insert("app_id", applicationInfo.title() + " " + applicationInfo.version());
	variablesBase.insert("platform+version", "bb10" + " " + platformInfo.osVersion);
}

void TealiumTagger::initializeWebView(){
	logger("initializeWebView", 1);
	if(initialWebViewState == -1){
		if(connectionTest()){
			try {
				QString path = CDN_ROOT + this->configAccount + "/" + this->configProfile + "/" + this->configEnvironment + "/mobile.html?v=56";
				initialWebViewState = 0;
				logger("WEBVIEW PATH -> " + path, 2);
				webView->setUrl(QUrl(path));
				webView->reload();
			}catch(const std::exception& ex){}
		}else if(statusAwake){
			logger("RETRYING INITIAL WEBVIEW IN 2 SECONDS", 3);
			QTimer::singleShot(2000, this, SLOT(initializeWebViewRetry()));
		}
	}
}

void TealiumTagger::initializeWebViewRetry(){
	if(!statusAwake){
		return;
	}

	logger("initializeWebViewRetry", 1);
	if(!initialWebView && initialWebViewState != 0){
		initialWebViewState = -1;
		initializeWebView();
	}
}

void TealiumTagger::onLoadingChanged(bb::cascades::WebLoadRequest *loadRequest){
	if (loadRequest->status() == bb::cascades::WebLoadStatus::Started) {
		logger("WEBVIEW STARTED", 3);
		initialWebViewState = 0;

	}else if (loadRequest->status() == bb::cascades::WebLoadStatus::Succeeded) {
		logger("WEBVIEW SUCCEEDED", 3);
		initialWebViewState = 1;
		initialWebView = true;
		sendRequestQueue();
    }else if(loadRequest->status() == bb::cascades::WebLoadStatus::Failed) {
		logger("WEBVIEW FAILED", 3);
		initialWebViewState = -1;
    }
}

void TealiumTagger::setVariables(Map variables){
	if(!variables.empty()) {
		variablesQueued.clear();
		return;
	}
	variablesQueued = Map(variables);
}

void TealiumTagger::setVariable(QString name, QString value){
	if(variablesQueued.empty()){
		variablesQueued = Map();
	}
	variablesQueued.insert(name, value);
}

void TealiumTagger::trackItemClicked(QString itemName){
	Map variables = Map();
	trackItemClicked(itemName, variables);
}

void TealiumTagger::trackItemClicked(QString itemName, Map variables){
	variables.insert("link_id", itemName);
	trackCustomEvent("link", variables);
}

void TealiumTagger::trackScreenViewed(QString viewName){
	Map variables = Map();
	trackScreenViewed(viewName, variables);
}

void TealiumTagger::trackScreenViewed(QString viewName, Map variables){
	variables.insert("screen_title", viewName);
	trackCustomEvent("view", variables);
}

void TealiumTagger::trackCustomEvent(QString eventName, Map variables){
	logger("TRACKING EVENT -> " + eventName, 2);
	QMap <QString, QVariant> variablesToSend = QMap<QString, QVariant>();
	if (!variablesBase.empty()) {
		Map::iterator i;
		for(i = variablesBase.begin(); i != variablesBase.end(); ++i){
			variablesToSend.insert(i.key(), i.value());
		}
	}

	if (!variablesQueued.empty()) {
		Map::iterator i;
		for(i = variablesQueued.begin(); i != variablesQueued.end(); ++i){
			variablesToSend.insert(i.key(), i.value());
		}
	}

	if (!variables.empty()) {
		Map::iterator i;
		for(i = variables.begin(); i != variables.end(); ++i){
			variablesToSend.insert(i.key(), i.value());
		}
	}

	QVariant qv = QVariant(variablesToSend);
	QString* jsonPtr = new QString();
	bb::data::JsonDataAccess jda;
	jda.saveToBuffer(qv, jsonPtr);
	QString jsonStr = *(jsonPtr);
	QString utagCommand = "javascript:utag.track('" + eventName + "'," + jsonStr.simplified() + ");";
	requestQueue.append(utagCommand);
	if(initialWebView){
		sendRequestQueue();
	}
}

void TealiumTagger::sendRequestQueue(){
	if(statusAwake){
		logger("IN sendRequestQueue", 1);
		if(!initialWebView){
			initializeWebViewRetry();
			return;
		}

		if(!sendingRequestItems && requestQueue.size() > 0){
			int sendTime = SEND_TIME_MIN + (rand() % SEND_TIME_VAR);
			logger("ITEMS ARE IN requestQueue, SENDING IN -> " + QString::number(sendTime), 3);
			QTimer::singleShot(sendTime, this, SLOT(sendRequestItem()));
		}
	}
}

void TealiumTagger::sendRequestItem(){
	logger("IN sendRequestItem", 1);
	if(statusAwake && this->connectionTest()){
		this->sendingRequestItems = true;
		QString req = this->requestQueue.takeFirst();
		this->webView->evaluateJavaScript(req, bb::cascades::JavaScriptWorld::Normal);
		this->logger("evaluateJavaScript IS COMPLETE", 2);
		this->sendingRequestItems = false;
		if(requestQueue.size() > 0){
			this->sendRequestQueue();
		}
	}
}

void TealiumTagger::connectionChange(){
	logger("CONNECTION CHANGED", 2);
	if(requestQueue.size() > 0){
		bool connectionPrevious = connectionCurrent;
		connectionCurrent = connectionTest();

		if (!connectionPrevious && connectionCurrent) {
			logger("REGAINED CONNECTIVITY -> sendRequestQueue", 3);
			sendRequestQueue();
		} else if(connectionPrevious && !connectionCurrent) {
			logger("LOST CONNECTIVITY -> SCHEDULE connectionChange", 3);
			QTimer::singleShot(2000, this, SLOT(connectionChange()));
		}
	}
}

bool TealiumTagger::connectionTest(){
	netstatus_get_availability(&connectionCurrent);
	if(connectionCurrent){
		logger("TESTING CONNECTION: true", 3);
	}else{
		logger("TESTING CONNECTION: false", 3);
	}
	return connectionCurrent;
}

void TealiumTagger::saveQueueToDisk(){
	if(requestQueue.size() <= 0){
		return;
	}

	logger("WRITING SEND_QUEUE_FILE", 1);
	QFile textfile(SEND_QUEUE_FILE);
	textfile.open(QIODevice::WriteOnly);
	logger(textfile.fileName(),1);
	QFileInfo info1(SEND_QUEUE_FILE);
	logger(info1.absoluteFilePath(), 1);
	QTextStream out(&textfile);
	while(requestQueue.size() > 0){
		out << requestQueue.takeFirst() << "\n";
	}
	textfile.close();
}

void TealiumTagger::loadQueueFromDisk(){
	 logger("loadQueueFromDisk", 1);

	 QFile file(SEND_QUEUE_FILE);
	 if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		 logger("SEND_QUEUE_FILE DOESNT EXIST",1);
		 return;
	 }

	 logger("READING SEND_QUEUE_FILE",1);
	 QTextStream in(&file);
	 QString line = in.readLine();
	 while (!line.isNull()) {
		 logger("ADDING LINE: " + line, 3);
		 requestQueue.append(line);
		 line = in.readLine();
	 }
	 file.remove();
}

void TealiumTagger::sleep(){
	statusAwake = false;
	saveQueueToDisk();
}

void TealiumTagger::wakeUp(){
	loadQueueFromDisk();
	statusAwake = true;
	sendRequestQueue();
}

void TealiumTagger::onAsleep() {
	logger("GOING TO SLEEP", 2);
	sleep();
}

void TealiumTagger::onAwake() {
	if(statusAwake == false){
		logger("WAKING UP", 2);
		wakeUp();
	}
}

void TealiumTagger::onAboutToQuit() {
    logger("EXITING", 2);
    sleep();
}

void TealiumTagger::logger(QString msg, int level){
	if(LOGGER_LEVEL >= level){
		qDebug() << "LOGGER: " << msg;
	}
}

