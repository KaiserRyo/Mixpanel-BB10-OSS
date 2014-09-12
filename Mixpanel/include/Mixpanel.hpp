/*
 * Mixpanel.hpp
 *
 *  Created on: 19 Aug 2014
 *      Author: JAragon
 */

#ifndef MIXPANEL_HPP_
#define MIXPANEL_HPP_

#include "mixpanel_global.hpp"

#include "../include/MixpanelConfiguration.hpp"

#include <QObject>
#include <QByteArray>


class MixpanelPrivate;
class MixpanelPeople;
class MixpanelEvent;
class MixpanelMessageQueue;

/// \mainpage Mixpanel BB10 Documentation
///
/// Mixpanel for BB10 is a lite version of Mixpanel for BB10. It is build to be integrated into BB10 Native apps that make use of BlackBerry-Cascades framework.
///
/// \section Features
///  - Track events
///  - Profile updates
///   - Set properties
///   - Increment properties
///   - Use super properties
///  - Default profile data (app version, OS version, Model...)
///  - Different flush configurations
///
///  \section Project resources:
///  - Project Page: https://github.com/trulabs/Mixpanel-BB10
///  - Project Issues: https://github.com/trulabs/Mixpanel-BB10/issues
///  - Project Releases:


/// \brief The Mixpanel class is the main class for using Mixpanel-BB10.
///
/// It provides the user all the required functionality to send
/// event and profile messages to Mixpanel.
///
/// \note This library is based on the Mixpanel HTTP Tracking API (https://mixpanel.com/help/reference/http)
///
/// By default the library will use its own configuration, but it is advised to create a custom configuration
/// and create the Mixpanel object directly with that configuration.
///
/// Mixpanel supported features:
///     -Identify: Associate future engage messages with a particular People Analytics user (see MixpanelPeople)
///
///     -Track events: in order to track events use trackEvent (see MixpanelEvent)
///
///     -Engage profile:
///             - Set properties: use setProfileProperties / setProfileProperty
///             - Increment property: use incrementProfileProperty
///
///     -Super properties: The super properties will be saved persistently and will be used for every event (see MixpanelPersistentIdentity)
///
///     -Default profile information: In every engage profile analytic message Mixpanel BB10 library will add default information as device model,
///                                   OS version, App version...
///
///     -Configurations: The library allows to use different configuration to match the needs (see MixpanelConfiguration)
///

class MIXPANEL_EXPORT Mixpanel: public QObject
{
    Q_OBJECT
public:

    explicit Mixpanel(QObject *parent = 0, MixpanelConfiguration config = MixpanelConfiguration());
    ~Mixpanel();

    void setConfiguration(const MixpanelConfiguration& config);
    void setToken(const QString& token);
    void setEventDistinctId(const QString& distinctId);

    MixpanelPeople& people() const;
    MixpanelEvent& event() const;
    MixpanelMessageQueue& messageQueue() const;

public slots:
    void identify(const QString& distinctId);

    void setProfileProperties(const QVariantMap& properties);
    void setProfileProperty(const QString& propertyName, const QVariant& value);

    void setOnceProfileProperties(const QVariantMap& properties);
    void setOnceProfileProperty(const QString& propertyName, const QVariant& value);

    void incrementProfileProperty(const QString& property, const double& value);

    void deleteProfile();

    void registerSuperProperties(const QVariantMap& superProperties);
    void registerSuperPropertiesOnce(const QVariantMap& superProperties);

    void unregisterSuperProperty(const QString& superPropertyName);
    void unregisterAllSuperProperties();

    void trackEvent(const QString& eventName, const QVariantMap& properties = QVariantMap());

    void flush();

    static QString convertToMixpanelDateFormat(const QDateTime& dateTime);

private:
    MixpanelPrivate * const d;
};

#endif /* MIXPANEL_HPP_ */
