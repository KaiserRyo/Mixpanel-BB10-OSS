/*
 * MixpanelEvent.cpp
 *
 *  Created on: 20 Aug 2014
 *      Author: JAragon
 */

#include "../include/MixpanelEvent.hpp"
#include "../include/MixpanelPersistentIdentity.hpp"

#include <bb/data/JsonDataAccess>
#include <QDateTime>


using namespace bb::data;


class MixpanelEventPrivate {
public:
    MixpanelPersistentIdentity persistentIdentity;

};

/// Creates a MixpanelEvent object.

MixpanelEvent::MixpanelEvent(QObject* parent)
    : QObject(parent)
    , d(new MixpanelEventPrivate)
{

}

/// Destructor, destroys the MixpanelEvent object.
///

MixpanelEvent::~MixpanelEvent()
{
    delete d;
}

/// Sets the distinct id for the events.
///
/// \param token distinct id to use on the events
///

void MixpanelEvent::setDistinctId(const QString& distinctId)
{
    d->persistentIdentity.setEventDistinctId(distinctId);
}

/// Returns the distinct id used for the events.
///
/// \return event distinct id
///

QString MixpanelEvent::distinctId() const
{
    return d->persistentIdentity.eventDistinctId();
}

/// Returns the persistent identity used for the events.
///
/// \return persistent identity
///

MixpanelPersistentIdentity& MixpanelEvent::persistentIdentity()
{
    return d->persistentIdentity;
}

///
/// Track an event.
///
/// \note Every call to track eventually results in a data point sent to Mixpanel. These data points
/// are what are measured, counted, and broken down to create your Mixpanel reports. Events
/// have a string name, and an optional set of name/value pairs that describe the properties of
/// that event.
///
/// \param eventName The name of the event to send
/// \param properties A QVariantMap containing the key value pairs of the properties to include in this event.
///                   Pass an empty QVariantMap if no extra properties exist.
////

void MixpanelEvent::track(const QString& name, const QVariantMap& properties)
{
    if (eventHasErrors(name))
    {
        qWarning() << "Event invalid -> Analytic message not recorded";
        return;
    }

    QVariantMap updatedProperties(properties);
    updatedProperties.unite(d->persistentIdentity.referrerProperties());
    updatedProperties.unite(d->persistentIdentity.eventSuperProperties());

    QByteArray eventData = stdTrackEvent(name, updatedProperties);

    if (!eventData.isEmpty())
        emit recordEventMessage(eventData);
    else
        emit trackError(InvalidJson, name);
}

/// Returns whether the event has errors
///
/// \return True if there is any errors
///

bool MixpanelEvent::eventHasErrors(const QString& eventName)
{
    if (d->persistentIdentity.token().isEmpty())
    {
        emit trackError(InvalidToken, eventName);
        return true;
    }

    if (eventName.isEmpty())
    {
        emit trackError(InvalidName, eventName);
        return true;
    }

    if (d->persistentIdentity.token().isEmpty())
    {
        emit trackError(InvalidIdentity, eventName);
        return true;
    }

    return false;
}

///
/// Creates a standar raw event message.
///
/// \param name The name of the event to send
/// \param properties A QVariantMap containing the key value pairs of the properties to include in this event.
////

QByteArray MixpanelEvent::stdTrackEvent(const QString& name, const QVariantMap& properties) const
{
    QVariantMap eventProperties(properties);
    QVariantMap eventData;
    JsonDataAccess dataAccess;
    QByteArray eventMessageData;

    eventProperties.insert("token", d->persistentIdentity.token());

    if(!d->persistentIdentity.eventDistinctId().isEmpty())
        eventProperties.insert("distinct_id", d->persistentIdentity.eventDistinctId());

    eventProperties.insert("time", QDateTime::currentDateTimeUtc().toMSecsSinceEpoch() / 1000);

    eventData.insert("event", name);
    eventData.insert("properties", eventProperties);

    dataAccess.saveToBuffer(eventData, &eventMessageData);

    if (dataAccess.hasError())
    {
        qWarning() << "JsonDataAccess has errors";
        return QByteArray();
    }

    return eventMessageData;
}




