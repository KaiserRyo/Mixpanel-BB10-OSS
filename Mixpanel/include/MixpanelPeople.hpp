/*
 * MixpanelPeople.hpp
 *
 *  Created on: 22 Jul 2014
 *      Author: JAragon
 */

#ifndef MIXPANELPEOPLE_HPP_
#define MIXPANELPEOPLE_HPP_


#include <QSharedDataPointer>
#include <QObject>
#include <QVariant>

#include "MixpanelPersistentIdentity.hpp"

class MixpanelPeoplePrivate;

///
/// \brief  The MixpanelPeople class provides an interface for using Mixpanel People Analytics features.
///
/// \note The People object is used to update properties in a user's People Analytics record,
/// and to manage the receipt of push notifications sent via Mixpanel Engage.
/// For this reason, it's important to call identify(QString) on the People
/// object before you work with it. Once you call identify, the user identity will
/// persist across stops and starts of your application, until you make another
/// call to identify using a different id.
///

class MixpanelPeople : public QObject
{
    Q_OBJECT

public:

    /// An enumeration for possible people errors
    enum EngageProfileError
    {
       InvalidToken = 0,   ///< The token is invalid
       InvalidIdentity,    ///< The profile update contained an invalid identity
       InvalidIncrement,   ///< The increment contains invalid data
       InvalidJson         ///< JSON conversation failed
    };

    MixpanelPeople(QObject* parent);
    ~MixpanelPeople();

    MixpanelPersistentIdentity& persistentIdentity();

    void identify(const QString& distinctId);

    void setDistinctId(const QString& distinctId);

    void set(const QVariantMap& properties);
    void set(const QString& propertyName, const QVariant& value);

    void setOnce(const QVariantMap& properties);
    void setOnce(const QString& propertyName, const QVariant& value);


    void increment(const QString& propertyName, double value);

    QByteArray stdPeopleMessage(const QString& action, const QVariantMap& properties);

    QString distinctId() const;

private:
    void engageProfileMessage(const QString& action, const QVariantMap& properties);
    bool engageHasErrors(const QString& action, const QVariantMap& properties);

signals:

    /// This signal is emitted when a valid profile engage analytic message
    /// has to be recorded in order to be posted to the Mixpanel server.
    ///
    void recordPeopleMessage(QByteArray message);

    /// This signal is emitted when in invalid event message has
    /// failed to be recorded due an error.
    ///
    void engageProfileError(const MixpanelPeople::EngageProfileError errorId, const QString action, const QVariantMap properties);

private:
    MixpanelPeoplePrivate * const d;

};
#endif /* MIXPANELPEOPLE_HPP_ */
