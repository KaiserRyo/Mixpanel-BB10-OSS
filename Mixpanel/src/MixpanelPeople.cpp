/*
 * MixpanelPeople.cpp
 *
 *  Created on: 22 Jul 2014
 *      Author: JAragon
 */

#include "../include/MixpanelPeople.hpp"

#include <bb/data/JsonDataAccess>
#include <QDateTime>

using namespace bb::data;

class MixpanelPeoplePrivate
{

public:
    MixpanelPersistentIdentity persistentIdentity;
};

/// Creates a MixpanelPeople object.


MixpanelPeople::MixpanelPeople(QObject *parent)
    : QObject(parent)
    , d(new MixpanelPeoplePrivate)
{

}

/// Destructor, destroys the MixpanelPeople object.
///

MixpanelPeople::~MixpanelPeople()
{
    delete d;
}

/// Returns the persistent identity used for the profile updates.
///
/// \return persistent identity
///

MixpanelPersistentIdentity& MixpanelPeople::persistentIdentity()
{
    return d->persistentIdentity;
}

///
/// Associate future calls to set(QVariantMap) and increment(QvariantMap),
/// with a particular People Analytics user.
///
/// \note All future calls to the People object will rely on this value to assign
/// and increment properties. The user identification will persist across
/// restarts of your application. We recommend calling
/// MixpanelPeople.identify as soon as you know the distinct id of the user.
///
/// \param distinctId a String that uniquely identifies the user. Users identified with
///     the same distinct id will be considered to be the same user in Mixpanel,
///     across all platforms and devices. We recommend choosing a distinct id
///     that is meaningful to your other systems (for example, a server-side account
///     identifier).
///

void MixpanelPeople::identify(const QString& distinctId)
{
    setDistinctId(distinctId);
    set(QVariantMap());
}

/// Sets the distinct id for the profile updates.
///
/// \param token distinct id to use on the profile updates
///

void MixpanelPeople::setDistinctId(const QString& distinctId)
{
    d->persistentIdentity.setPeopleDisctinctId(distinctId);
}

/// Returns the distinct id used for the profile updates.
///
/// \return people distinct id
///

QString MixpanelPeople::distinctId() const
{
    return d->persistentIdentity.peopleDistinctId();
}

///
/// Set a collection of properties on the identified user all at once.
///
/// \param properties a QVariantMap containing the collection of properties you wish to apply
///      to the identified user. Each key in the JSONObject will be associated with
///      a property name, and the value of that key will be assigned to the property.
///

void MixpanelPeople::set(const QVariantMap& properties)
{
    QVariantMap updatedProperties(properties);
    updatedProperties.unite(d->persistentIdentity.referrerProperties());

    engageProfileMessage("$set", updatedProperties);
}

///
/// Sets a single property with the given name and value for this user.
/// The given name and value will be assigned to the user in Mixpanel People Analytics,
/// possibly overwriting an existing property with the same name.
///
/// \param propertyName The name of the Mixpanel property. This must be a QString, for example "Zip Code"
/// \param value The value of the Mixpanel property. For "Zip Code", this value might be the QString "90210"
///

void MixpanelPeople::set(const QString& propertyName, const QVariant& value)
{
    QVariantMap dataMap;
    dataMap.insert(propertyName, value);

    set(dataMap);
}

///
/// Works just like set, except it will not overwrite existing property values. This is useful for properties like "First login date".
///
/// \param properties a QVariantMap containing the collection of properties you wish to apply
///      to the identified user. Each key in the JSONObject will be associated with
///      a property name, and the value of that key will be assigned to the property.
///

void MixpanelPeople::setOnce(const QVariantMap& properties)
{
    engageProfileMessage("$set_once", properties);
}


///
/// Works just like set, except it will not overwrite existing property values. This is useful for properties like "First login date".
///
/// \param propertyName The name of the Mixpanel property. This must be a QString, for example "Zip Code"
/// \param value The value of the Mixpanel property. For "Zip Code", this value might be the QString "90210"
///

void MixpanelPeople::setOnce(const QString& propertyName, const QVariant& value)
{
    QVariantMap dataMap;
    dataMap.insert(propertyName, value);

    setOnce(dataMap);
}


/// Records a analytic profile update message using an action given
///
/// \param actionName The profile operation or action ($set_once, $append...)
/// \param actionProperties The properties inside the action
///

void MixpanelPeople::setCustomAction(const QString& actionName, const QVariantMap& actionProperies)
{
    engageProfileMessage(actionName, actionProperies);
}

///
/// Add the given amount to an existing property on the identified user. If the user does not already
/// have the associated property, the amount will be added to zero. To reduce a property,
/// provide a negative number for the value.
///
/// \param propertyName name of the People Analytics property that should have its value changed
/// \param value the amount to be added to the current value of the named property
///

void MixpanelPeople::increment(const QString& propertyName, double value)
{
    QVariantMap properties;
    properties.insert(propertyName, QVariant(value));

    if (propertyName.isEmpty())
    {
        qWarning() << "Invalid increment property name -> Analytic message not recorded";
        emit engageProfileError(InvalidIncrement, "$add", properties);
        return;
    }

    engageProfileMessage("$add", properties);
}

/// Records a delete user mixpanel analytics message

void MixpanelPeople::deleteUser()
{
    engageProfileMessage("$delete", QVariantMap());
}

/// Prepares the engage analytic message to be recorded
///
/// \note It checks whether there is any error. If the message is correct
///  it emits the signal to record the message
///
/// \param action is the action type of the engage message
/// \param properties The properties to be contained in the engage message
///

void MixpanelPeople::engageProfileMessage(const QString& action, const QVariantMap& properties)
{
    if (engageHasErrors(action, properties))
    {
        qWarning() << "Profile update invalid -> Analytic message not recorded";
        return;
    }

    QByteArray peopleMessageData = stdPeopleMessage(action, properties);

    if (!peopleMessageData.isEmpty())
        emit recordPeopleMessage(peopleMessageData);
    else
        emit engageProfileError(InvalidJson, action, properties);
}

/// Returns whether the engage action has any errors
///
/// \return True if there are any errors
///

bool MixpanelPeople::engageHasErrors(const QString& action, const QVariantMap& properties)
{
    if (d->persistentIdentity.token().isEmpty())
    {
        emit engageProfileError(InvalidToken, action, properties);
        return true;
    }

    if (d->persistentIdentity.token().isEmpty())
    {
        emit engageProfileError(InvalidIdentity, action, properties);
        return true;
    }

    if (action.isEmpty())
    {
        emit engageProfileError(IvalidAction, action, properties);
        return true;
    }

    return false;
}

///
/// Creates a standar raw profile analytics message.
///
/// \param name The action to be perform in Mixpanel
/// \param properties A QVariantMap containing the key value pairs of the properties to include in this profile update.
////

QByteArray MixpanelPeople::stdPeopleMessage(const QString& action, const QVariantMap& properties)
{
    QVariantMap dataMap;
    JsonDataAccess dataAccess;
    QByteArray peopleMessageData;

    dataMap.insert(action, properties);

    dataMap.insert("$token", d->persistentIdentity.token());
    dataMap.insert("$distinct_id", d->persistentIdentity.peopleDistinctId());
    dataMap.insert("$time", QDateTime::currentDateTimeUtc().toMSecsSinceEpoch());

    dataAccess.saveToBuffer(dataMap, &peopleMessageData);

    if (dataAccess.hasError())
    {
        qWarning() << "JsonDataAccess has errors";
        return QByteArray();
    }

    return peopleMessageData;
}


