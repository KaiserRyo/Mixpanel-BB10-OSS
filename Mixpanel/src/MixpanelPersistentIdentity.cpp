/*
 * PersistentIdentity.cpp
 *
 *  Created on: 21 Aug 2014
 *      Author: JAragon
 */

#include "../include/MixpanelPersistentIdentity.hpp"

#include "../include/MixpanelConstants.hpp"

#include <bb/device/HardwareInfo>
#include <bb/ApplicationInfo>
#include <bb/platform/PlatformInfo>

class MixpanelPersistentIdentityPrivate : public QSharedData
{
public:
    QString token;
    QString eventDistinctId;
    QString peopleDistinctId;
    QVariantMap referrerProperties;
    QVariantMap superPropertiesCache;
};

/// Creates a PersistentIdentity object.

MixpanelPersistentIdentity::MixpanelPersistentIdentity()
    : d(new MixpanelPersistentIdentityPrivate())
{

}

/// Creates a copy of \a other.

MixpanelPersistentIdentity::MixpanelPersistentIdentity(const MixpanelPersistentIdentity& other)
    : d(other.d)
{

}

/// Destructor, destroys the MixpanelPersistentIdentity object.
///

MixpanelPersistentIdentity::~MixpanelPersistentIdentity()
{

}

/// Assigns \a other to this PersistentIdentity

MixpanelPersistentIdentity& MixpanelPersistentIdentity::operator=(const MixpanelPersistentIdentity &other)
{
    d = other.d;
    return *this;
}


/// Sets the Mixpanel token.
///
/// \param token Token of the Mixpanel account
///

void MixpanelPersistentIdentity::setToken(const QString& token)
{
    d->token = token;
}

/// Sets the distinct id for the events.
///
/// \param token distinct id to use on the events
///

void MixpanelPersistentIdentity::setEventDistinctId(const QString& distinctId)
{
    d->eventDistinctId = distinctId;
}

/// Sets the distinct id for the profile updates.
///
/// \param token distinct id to use on the profile actions
///

void MixpanelPersistentIdentity::setPeopleDisctinctId(const QString& distinctId)
{
    if (!distinctId.isEmpty())
        d->peopleDistinctId = distinctId;

    saveDistinctPeopleId();
}

/// Returns the Mixpanel account token.
///
/// \return token
///

QString MixpanelPersistentIdentity::token() const
{
    return d->token;
}

/// Returns the distinct id used for the events.
///
/// \return event distinct id
///

QString MixpanelPersistentIdentity::eventDistinctId() const
{
    return d->eventDistinctId;
}

/// Returns the distinct id used for the profile.
///
/// \return profile distinct id
///

QString MixpanelPersistentIdentity::peopleDistinctId() const
{
    return d->peopleDistinctId;
}

/// Returns the referrer properties.
///
/// \return a QVariantMap containing the referrer properties
///

QVariantMap MixpanelPersistentIdentity::referrerProperties() const
{
    return d->referrerProperties;
}

/// Returns the event super properties.
///
/// \return a QVariantMap containing the event super properties
///

QVariantMap MixpanelPersistentIdentity::eventSuperProperties() const
{
    return d->superPropertiesCache;
}

///
/// Register properties that will be sent with every subsequent call to track events.
/// SuperProperties are a collection of properties that will be sent with every event to Mixpanel,
/// and persist beyond the lifetime of your application.
///
/// Setting a superProperty with registerSuperProperties will store a new superProperty,
/// possibly overwriting any existing superProperty with the same name (to set a
/// superProperty only if it is currently unset, use registerSuperPropertiesOnce(QVariantMap)
///
/// \note SuperProperties will persist even if your application is taken completely out of memory.
/// to remove a superProperty, call unregisterSuperProperty(QString) or clearSuperProperties()
///
/// \param superProperties    A QVariantMap containing super properties to register
///

void MixpanelPersistentIdentity::registerSuperProperties(const QVariantMap& superProperties)
{
    Q_FOREACH(QString propertyName, superProperties.keys())
    {
        if (!d->superPropertiesCache.contains(propertyName))
        {
            d->superPropertiesCache.insert(propertyName, superProperties.value(propertyName));
        } else {
            d->superPropertiesCache[propertyName] = superProperties.value(propertyName);
        }
    }

    saveSuperProperties();
}


///
/// Register super properties for events, only if no other super property with the
/// same names has already been registered.
///
/// \note Calling registerSuperPropertiesOnce will never overwrite existing properties.
///
/// \param superProperties A QVariantMap containing the super properties to register.
///

void MixpanelPersistentIdentity::registerSuperPropertiesOnce(const QVariantMap& superProperties)
{
    Q_FOREACH(QString propertyName, superProperties.keys())
    {
        if (!d->superPropertiesCache.contains(propertyName))
        {
            d->superPropertiesCache.insert(propertyName, superProperties.value(propertyName));
        }
    }

    saveSuperProperties();
}

///
/// Remove a single superProperty, so that it will not be sent with future calls to track events.
///
/// \note If there is a superProperty registered with the given name, it will be permanently
/// removed from the existing superProperties.
/// To clear all superProperties, use clearSuperProperties()
///
/// \param superPropertyName name of the property to unregister
///

void MixpanelPersistentIdentity::unregisterSuperProperty(const QString& superPropertyName)
{
    if (d->superPropertiesCache.contains(superPropertyName))
    {
        d->superPropertiesCache.remove(superPropertyName);
        saveSuperProperties();
    }
}

///
/// Erase all currently registered superProperties.
///
/// /note Future tracking calls to Mixpanel (even those already queued up but not
/// yet sent to Mixpanel servers) will not be associated with the superProperties registered
/// before this call was made.
///
/// /note To remove a single superProperty, use unregisterSuperProperty(QString)
///
///

void MixpanelPersistentIdentity::clearSuperProperties()
{
    QSettings settings(g_organizationName);
    settings.remove(g_superPropertiesKey);

    d->superPropertiesCache.clear();
}

/// Read the device identity and stores it into the referrerProperties.
/// It will set the event or people distinct id as the BB PIN if they haven't been set.
///

void MixpanelPersistentIdentity::readIdentities()
{
    bb::device::HardwareInfo hardwareInfo;
    bb::ApplicationInfo appInfo;
    bb::platform::PlatformInfo platformInfo;

    d->referrerProperties.insert("mp_lib", "blackberry");
    d->referrerProperties.insert("$os", "BB10");

    if (!platformInfo.osVersion().isEmpty())
        d->referrerProperties.insert("$os_version", platformInfo.osVersion());

    if (!appInfo.version().isEmpty())
        d->referrerProperties.insert("$app_version", appInfo.version());

    if (!hardwareInfo.deviceName().isEmpty())
        d->referrerProperties.insert("Device name", hardwareInfo.deviceName());

    if (!hardwareInfo.modelName().isEmpty())
        d->referrerProperties.insert("$model", hardwareInfo.modelName());

    if (!hardwareInfo.pin().isEmpty())
        d->referrerProperties.insert("PIN", hardwareInfo.pin());

    if (d->eventDistinctId.isEmpty())
        setEventDistinctId(hardwareInfo.pin());

    if (d->peopleDistinctId.isEmpty())
        setPeopleDisctinctId(hardwareInfo.pin());
}


///
/// Loads all the persistent data previously saved.
///

void MixpanelPersistentIdentity::loadPersistentData()
{
    QSettings settings(g_organizationName);
    d->superPropertiesCache = settings.value(g_superPropertiesKey, QVariantMap()).toMap();

    d->peopleDistinctId = settings.value(g_peopleDistinctIdKey, QString()).toString();
}

///
/// Save all the super properties registered to make them persistent.
///

void MixpanelPersistentIdentity::saveSuperProperties()
{
    QSettings settings(g_organizationName);
    settings.setValue(g_superPropertiesKey, d->superPropertiesCache);
}

///
/// Save the distinct people id to make it persistent.
///

void MixpanelPersistentIdentity::saveDistinctPeopleId()
{
    QSettings settings(g_organizationName);
    settings.setValue(g_peopleDistinctIdKey, d->peopleDistinctId);
}



