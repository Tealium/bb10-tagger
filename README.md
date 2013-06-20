Tealium BB10 Tagger - Version 1.0
=================================

This library framework provides Tealium customers the means to tag their native BB10 applications for the purpose of leveraging the vendor-neutral tag management platform offered by Tealium.  

The framework currently provides for:
- Analytics integration via the Tealium platform
- Easy to implement view tracking
- Easy to implement event tracking
- Custom tracking data assignments
- Offline caching
- Simple setup and remote configuration


These instructions presume you have valid accounts with both Tealium and the analytic services you will be mapping data to.

Implementation is a three part process:
1. Download and implement the library into your project
2. Configure your Tealium IQ


Table of Repo Contents
----------------------
- **bb10-tagger** folder contains all files and references necessary to run the Tealium BB10 Tagger in your app
- **bb10-tagger_apptest** folder contains a sample app for reference.
- **LICENSE** file is currently an empty file and can ignored
- **README.md** file is this document file


Requirements
--------------------
- An active Tealium IQ Account
- Your Tealium Account Id (it will likely be your company name)
- The Tealium Profile name to be associated with the app (your account may have several profiles, ideally one of them is dedicated to your BB10 app)
- The Tealium environment to use:
  - prod
  - qa
  - dev
- Blackberry's native ide - QNX Momentics 10.1.0+ recommended (https://developer.blackberry.com/native/download/)


Installation
------------
Installing the Tealium BB10 Tagger into your app requires the following steps:

  1. Import the following files from the bb10_tagger/src folder into your project's /src folder:
      - tealiumtagger.cpp
      - tealiumtagger.h
      <p></p>
  2. Add the following to your .pro file:
	```c++
	#ADDED BY TEALIUM
	LIBS += -lbbdata
	LIBS += -lbbplatform
	LIBS += -lbb
	```
	<p></p>
  3. Copy the config.xml file into your project


Initialization
--------------
In your app's .cpp file (not your main.cpp) add the following init line:
```c++
    TealiumTagger *TealiumTaggerObj = new TealiumTagger(bb::cascades::Application::instance(), "yourAccountName", "yourProfile", "yourTargetEnvironment");
```

How To Use
----------
The BB10 Tagger library dispatches a view, event or custom call depending on which of the below methods you use.  By default, view calls become screen_title data sources in Tealium's IQ, and event calls become link_id data sources. With any of these calls, you can also pass in a dictionary as an argument.  That dictionary's keys become data sources that you can map in Tealium's IQ to whatever vendor tag variables you wish.

### View Tracking

```c++
	void TealiumTagger::trackScreenViewed(QString viewName){

	void TealiumTagger::trackScreenViewed(QString viewName, Map variables){
```

For example:
```c++
 	
 	// Simple view call
 	tealiumtagger->trackScreenViewed("myViewName1");

 	// Advanced view call
 	QMap<QString,QString> customData;
    customData.insert("myDataSource1","myValue1");
    customData.insert("myDataSource2", "myValue2");
    tealiumtagger->trackScreenViewed("myViewName2", customData);

```

### Custom Item Click Tracking
Track any action or event with the following methods:

```c++
	void TealiumTagger::trackItemClicked(QString itemName)

	void TealiumTagger::trackItemClicked(QString itemName, Map variables){
```

For example:
```c++
 	
 	// Simple action call
 	tealiumtagger->trackItemClicked("myStringId1");

 	// Advanced action call
 	QMap<QString,QString> customData;
    customData.insert("myDataSource1","myValue1");
    customData.insert("myDataSource2", "myValue2");
    tealiumtagger->trackItemClicked("myStringId2", customData);

```

Default Data Sources
====================
The following key variables are available from the BB10 Library with each dispatch and simply need to be mapped to your analytic tag of choice within your Tealium IQ panel:

- app_id            A composite of app_name & app_version (ie myApp 1.0)
- app_name          Name of app
- app_version       Your app version number
- link_id           String value id from trackItemClicked's first argument
- os_version        Version of the OS (ie 10.1.0)
- screen_title      Page/View name
- pkg_author
- pkg_name
- pkg_version
- platform          Platform (ie bb10)
- platform+version  Composite of platform+os_version (bb10 10.1)


Troubleshooting
---------------
 
PROBLEM: The library doesn't appear to be working when I have printf() or fprintf() statements in my code
SOLUTION: There is an issue with Momentics 10.1.0 where an fprintf() statment will quietly disable the BB10 Tagger library and no message will print to the console.  No warning in the ide will appear in release or debug mode. Remove or comment out all printf() or fprintf statements.  qDebug() is the only logging method recommended.



Support
-------

For additional help and support, please send all inquires to mobile_support@tealium.com

