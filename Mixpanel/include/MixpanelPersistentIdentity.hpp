/*
 * PersistentIdentity.hpp
 *
 *  Created on: 21 Aug 2014
 *      Author: JAragon
 */

#ifndef PERSISTENTIDENTITY_HPP_
#define PERSISTENTIDENTITY_HPP_

#include <QSharedData>

class MixpanelPersistentIdentityPrivate;


/// \brief The MixpanelPersistentIdentity class holds the Mixpanel identity.
///
/// The Mixpanel Identity will be shared between MixpanelPeople and MixpanelEvent classes.
///
/// It is a container of the Mixpanel Identity that will be used to identify
/// the events and profiles updates within Mixpanel.
/// It contains the distincts ids, the token id, the super properties, and the referrer
/// properties.
///

class MixpanelPersistentIdentity
{
public:
    MixpanelPersistentIdentity();
    MixpanelPersistentIdentity(const MixpanelPersistentIdentity& other);
    virtual ~MixpanelPersistentIdentity();
    MixpanelPersistentIdentity& operator=(const MixpanelPersistentIdentity &other);

    QString token() const;
    void setToken(const QString&);

    QString eventDistinctId() const;
    void setEventDistinctId(const QString&);

    QString peopleDistinctId() const;
    void setPeopleDisctinctId(const QString&);

    void registerSuperProperties(const QVariantMap& superProperties);
    void registerSuperPropertiesOnce(const QVariantMap& superProperties);

    void unregisterSuperProperty(const QString& superPropertyName);

    void clearSuperProperties();

    QVariantMap referrerProperties() const;
    QVariantMap eventSuperProperties() const;


    void loadPersistentData();
    void readIdentities();

private:
    void saveSuperProperties();
    void saveDistinctPeopleId();

    QExplicitlySharedDataPointer<MixpanelPersistentIdentityPrivate> d;
};

#endif /* PERSISTENTIDENTITY_HPP_ */
