Mixpanel Library for BB10
===========

Mixpanel library for BlackBerry 10 (BB10) Cascades apps.
This library makes it easy to build BB10 apps that make use of Mixpanel analytics services.
For information about Mixpanel, refer to http://mixpanel.com/

Features
--------------------

+ Track events
+ Profile updates
  +  Set properties
  +  Increment properties
+ Use super properties
+ Default profile data (app version, OS version, Model...)
+ Different flush configurations


License
--------------------
BSD 3-Clause / new / simplified (see LICENSE)


Requirements
------------
+ BlackBerry Cascades Native SDK + QDE
+ A BlackBerry 10 Device or Simulator
+ A valid Mixpanel Token
+ App enabled permission in the bar-descriptor(See [MixpanelExample bar-descriptor](MixpanelExample/bar-descriptor.xml)): 
  + Device Identifying Information: read_device_identifying_information
  + Internet: access_internet
+ Libraries (.pro): 
  + lbbdevice lbbplatform lbb lbbdata  (See [MixpanelExample.pro](MixpanelExample/MixpanelExample.pro))
	

Building this Sample
--------------------
Import both Mixpanel and MixpanelExample projects into your NDK workspace. The Mixpanel project is the library itself and MixpanelExample is a simple application that shows how you might use the Mixpanel library.
You will need to update the variable token with  your own token in [applicationui.cpp](MixpanelExample/src/applicationui.cpp).


How to use For Your Own Application
-----------------------------------
In order to know how to use the library in your own application please refer to [Mixpanel readme](Mixpanel/readme.txt).
If you have any problem using the library please refer to the [Mixpanel example project](MixpanelExample) as a guidance. 
###Configure and start Mixpanel
Make sure you have added the library properly to your project as it is explained in the above paragraph.

    MixpanelConfiguration config;
    config.setFlushInterval(60000);
    config.setFlushMechanism(MixpanelConfiguration::Auto);
    config.setMessagesToFlush(10);
    config.setThumbnailFlush(true);

    Mixpanel* m_mixpanel = new Mixpanel(this, config);

    m_mixpanel->setToken("your Mixpanel token here");
    m_mixpanel->identify("");

Create your own slots and connect them to the the library signals:

	void mixpanelMessagePosted(const MixpanelMessageQueue::MixpanelPostMessageError errorCode, const QVariantMap analyticMessage);
	void mixpanelTrackError(MixpanelEvent::TrackEventError error, const QString eventName);
	void mixpanelEngageProfileError(const MixpanelPeople::EngageProfileError errorId, const QString action, const QVariantMap properties);
	
Somewhere you need to add the following connections:

    bool connectResult = false;
    Q_UNUSED(connectResult);

    connectResult = connect(&m_mixpanel->messageQueue(), SIGNAL(mixpanelMessagePosted(MixpanelMessageQueue::MixpanelPostMessageError,QVariantMap)), this, SLOT(mixpanelMessagePosted(MixpanelMessageQueue::MixpanelPostMessageError,QVariantMap)));
    Q_ASSERT(connectResult);

    connectResult = connect(&m_mixpanel->event(), SIGNAL(trackError(MixpanelEvent::TrackEventError,QString)), this, SLOT(mixpanelTrackError(MixpanelEvent::TrackEventError,QString)));
    Q_ASSERT(connectResult);

    connectResult = connect(&m_mixpanel->people(), SIGNAL(engageProfileError(MixpanelPeople::EngageProfileError,QString,QVariantMap)), this, SLOT(mixpanelEngageProfileError(MixpanelPeople::EngageProfileError,QString,QVariantMap)));
    Q_ASSERT(connectResult);

###Set a profile update in C++
To set a profile update you just need to create a QVariantMap with your the properties and use setProfileProperties:
    
    QVariantMap properties;
    properties.insert("$phone", "07987645159");
    properties.insert("$username", "lionelMessi");
    properties.insert("$first_name", "Leo");
    properties.insert("$last_name", "Messi");
    properties.insert("$email", "leomessi@gmail.com");
    m_mixpanel->setProfileProperties(properties);

###Track a event in QML

Set an instance of mixpanel as a context property: 
    
    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    qml->setContextProperty("app", this);
    qml->setContextProperty("mixpanel", m_mixpanel);

    // Set created root object as the application scene
    Application::instance()->setScene(root);


Track event in a QML Button:

	Button {
		text: "Track Level complete"
		onClicked: {
		    var eventPorperties = {
		        "Level Number" : 9,
		        "Coins" : 1250
		    }
		    mixpanel.trackEvent("Level Complete", eventPorperties);                    
		}
	}


Documentation
-------------
Documentation is provided by doxygen, use doxygen Doxyfile to generate the html documentation under /doc directory.


