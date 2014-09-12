/*
 * Mixpanel.cpp
 *
 *  Created on: 19 Aug 2014
 *      Author: JAragon
 */

#include "../include/Mixpanel.hpp"
#include "../include/MixpanelPeople.hpp"
#include "../include/MixpanelEvent.hpp"
#include "../include/MixpanelPersistentIdentity.hpp"
#include "../include/MixpanelMessageQueue.hpp"

#include "qdebug.h"

class MixpanelPrivate {
public:
    MixpanelPrivate(Mixpanel *qq);

    MixpanelPeople *mixpanelPeople;
    MixpanelEvent *mixpanelEvent;
    MixpanelPersistentIdentity persistentIdentity;
    MixpanelMessageQueue *messageQueue;

private:
    Mixpanel *q;
};


MixpanelPrivate::MixpanelPrivate(Mixpanel *qq)
    : mixpanelPeople(0)
    , mixpanelEvent(0)
    , messageQueue(0)
    , q(qq)
{

}

/// Creates a Mixpanel object.
/// \param parent is passed to the QObject's constructor.
/// The default value is 0.

Mixpanel::Mixpanel(QObject *parent, MixpanelConfiguration config)
    : QObject(parent)
    , d(new MixpanelPrivate(this))
{
    d->mixpanelPeople = new MixpanelPeople(this);
    d->mixpanelEvent = new MixpanelEvent(this);
    d->messageQueue = new MixpanelMessageQueue(this, config);

    d->persistentIdentity.loadPersistentData();
    d->persistentIdentity.readIdentities();

    d->mixpanelEvent->persistentIdentity() = d->persistentIdentity;
    d->mixpanelPeople->persistentIdentity() = d->persistentIdentity;

    bool connectResult = false;
    Q_UNUSED(connectResult);

    connectResult = connect(d->mixpanelPeople, SIGNAL(recordPeopleMessage(QByteArray)), d->messageQueue, SLOT(recordPeopleMessage(QByteArray)));
    Q_ASSERT(connectResult);

    connectResult = connect(d->mixpanelEvent, SIGNAL(recordEventMessage(QByteArray)), d->messageQueue, SLOT(recordEventMessage(QByteArray)));
    Q_ASSERT(connectResult);

}

/// Destructor, destroys the Mixpanel object.
///

Mixpanel::~Mixpanel()
{
    delete d;
}

/// Sets the Mixpanel configuration
///
/// \param Mixpanel configuration
///

void Mixpanel::setConfiguration(const MixpanelConfiguration& config)
{
    d->messageQueue->setConfiguration(config);
}

/// Sets the Mixpanel token linked to the mixpanel account.
///
/// \param Mixpanel token, all the network request will contain this token
///

void Mixpanel::setToken(const QString& token)
{
    d->persistentIdentity.setToken(token);
}


/// Sets the Distinct id to be used in the Mixpanel event requests.
///
/// \param Events distinct id
///

void Mixpanel::setEventDistinctId(const QString& distinctId)
{
    d->mixpanelEvent->setDistinctId(distinctId);
}

void Mixpanel::identify(const QString& distinctId)
{
    d->mixpanelPeople->identify(distinctId);
}

/// Sets the Distinct id to be used in the Mixpanel event requests.
///
/// \param Events distinct id
///

void Mixpanel::setProfileProperties(const QVariantMap& properties)
{
    d->mixpanelPeople->set(properties);
}

/// Sets the new property in the profile.
///
/// \param property name as a QString
/// \param object property as a QVariant
///

void Mixpanel::setProfileProperty(const QString& property, const QVariant& object)
{
    d->mixpanelPeople->set(property, object);
}

void Mixpanel::setOnceProfileProperties(const QVariantMap& properties)
{
    d->mixpanelPeople->setOnce(properties);
}

void Mixpanel::setOnceProfileProperty(const QString& propertyName, const QVariant& propertyValue)
{
    d->mixpanelPeople->setOnce(propertyName, propertyValue);
}

/// Registers super properites in the persistent identity
///
/// \param superProperties as QVariantMap
///

void Mixpanel::registerSuperProperties(const QVariantMap& superProperties)
{
    d->persistentIdentity.registerSuperProperties(superProperties);
}

/// Registers super properites in the persistent identity only if
/// the super property does not exist.
/// \param superProperties as QVariantMap
///

void Mixpanel::registerSuperPropertiesOnce(const QVariantMap& superProperties)
{
    d->persistentIdentity.registerSuperPropertiesOnce(superProperties);
}

/// Unregisters a super property in the persistent identity.
/// \param superPropertyName as QString
///

void Mixpanel::unregisterSuperProperty(const QString& superPropertyName)
{
    d->persistentIdentity.unregisterSuperProperty(superPropertyName);
}

/// Unregisters all the super properties stored in the persistent identity.
///

void Mixpanel::unregisterAllSuperProperties()
{
    d->persistentIdentity.clearSuperProperties();
}

/// Tracks an event to Mixpanel server.
/// \param eventName as QString
/// \param properties as QVaraintMap containing the event properties
///

void Mixpanel::trackEvent(const QString& eventName, const QVariantMap& properties)
{
    d->mixpanelEvent->track(eventName, properties);
}

/// Sends a profile "add" update to Mixpanel.
/// \param property as QString containing the property name
/// \param value as Double containing the amount to be increased
///

void Mixpanel::incrementProfileProperty(const QString& property, const double& value)
{
    d->mixpanelPeople->increment(property, value);
}

/// Flushes all messages in the message queue to the Mixpanel server

void Mixpanel::flush()
{
    d->messageQueue->postToServer();
}

/// Returns the reference to MixpanelPeople

MixpanelPeople& Mixpanel::people() const
{
    return *d->mixpanelPeople;
}

/// Returns the reference to MixpanelEvent

MixpanelEvent& Mixpanel::event() const
{
    return *d->mixpanelEvent;
}

/// Returns the reference to MixpanelMessageQueue

MixpanelMessageQueue& Mixpanel::messageQueue() const
{
    return *d->messageQueue;
}


