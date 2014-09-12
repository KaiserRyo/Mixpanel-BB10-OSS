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
  +  Set once properties
  +  Increment properties
  +  Delete user profile
  +  Any set Mixpanel operation
+ Use super properties
+ Default profile data (app version, OS version, Model...)
+ Different flush configurations


License
--------------------
[BSD 3-Clause / new / simplified](http://opensource.org/licenses/BSD-3-Clause) (see [LICENSE](LICENSE))


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

We recommend to use the library in QML, to do that you need to set an instance of mixpanel as a context property: 
    
    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    qml->setContextProperty("app", this);
    qml->setContextProperty("mixpanel", m_mixpanel);

    // Set created root object as the application scene
    Application::instance()->setScene(root);

If you want to listen to the Mixpanel library signals to know whether your data has been propertly sent you need to create your own slots and connect them to the the library signals:

    void mixpanelMessagePosted(const MixpanelMessageQueue::MixpanelPostMessageError errorCode, const QVariantMap analyticMessage);
    void mixpanelTrackError(MixpanelEvent::TrackEventError errorCode, const QString eventName, const QVariantMap eventProperties);
    void mixpanelEngageProfileError(const MixpanelPeople::EngageProfileError errorCode, const QString action, const QVariantMap properties);
	
Somewhere you need to add the following connections:

    bool connectResult = false;
    Q_UNUSED(connectResult);

    connectResult = connect(&m_mixpanel->messageQueue(), SIGNAL(mixpanelMessagePosted(MixpanelMessageQueue::MixpanelPostMessageError,QVariantMap)), this, SLOT(mixpanelMessagePosted(MixpanelMessageQueue::MixpanelPostMessageError,QVariantMap)));
    Q_ASSERT(connectResult);

    connectResult = connect(&m_mixpanel->event(), SIGNAL(trackError(MixpanelEvent::TrackEventError,QString,QVariantMap)), this, SLOT(mixpanelTrackError(MixpanelEvent::TrackEventError,QString,QVariantMap)));
    Q_ASSERT(connectResult);

    connectResult = connect(&m_mixpanel->people(), SIGNAL(engageProfileError(MixpanelPeople::EngageProfileError,QString,QVariantMap)), this, SLOT(mixpanelEngageProfileError(MixpanelPeople::EngageProfileError,QString,QVariantMap)));
    Q_ASSERT(connectResult);

###Set a profile updates

As we said before we recommend the use of the library in your QML code. To set a profile update in QML you just need to create the a JS var containing the properties of the udpate and then use setProfileProperties. Lets see hook a profile update when the user tap a button:
	
	Button {
                text: "Set player properties"
                onClicked: {
                    
                    var properties = {
                        "Nickname": "Messiah",
                        "Position": [ "CF", "RF"],
                        "Number" : 10,
                        "Nationality" : "Argentina",
                        "Club" : "FC Barcelona"
                    }
                    mixpanel.setProfileProperties(properties)
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }

You can do exaclty the same thing in your C++ code. You just need to create a QVariantMap with your the properties and use setProfileProperties:
    
    QVariantMap properties;
    properties.insert("$phone", "07987645159");
    properties.insert("$username", "lionelMessi");
    properties.insert("$first_name", "Leo");
    properties.insert("$last_name", "Messi");
    properties.insert("$email", "leomessi@gmail.com");
    m_mixpanel->setProfileProperties(properties);

###Track a event

Tracking events is same simple as setting profile updates. In QML you just need to create the JS var and the name of your event. Then it is just to call trackEvent. Lets see how an event is hooked to a button:
Track event in a QML Button:

	Button {
		text: "Track Level complete"
		onClicked: {
		    var eventProperties = {
		        "Level Number" : 9,
		        "Coins" : 1250
		    }
		    mixpanel.trackEvent("Level Complete", eventProperties);                    
		}
	}


Tracking events is also available to use in your C++ code.

    QVariantMap eventProperties;
    eventProperties.insert("Level number", 11);
    eventProperties.insert("Coins", 1000);

    m_mixpanel->trackEvent("Level Complete", eventProperties);


###Use of non common update operations:

In order to use other update operation as $append, $union or $unset, you can use setCustomAction. Lets see how to unset some profile properties in QML:


	Button {
                text: "Unset player properties"
                onClicked: {

                    var unsetProperties = {
                        "$unset": [ "Number", "Nickname", "Nationality", "Position", "Club"]
                    }
                    mixpanel.setCustomAction(unsetProperties)
                }
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
            }


Documentation
-------------
Documentation is provided by doxygen, use doxygen Doxyfile to generate the html documentation under /doc directory.


