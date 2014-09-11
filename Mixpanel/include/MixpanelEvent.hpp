/*
 * MixpanelEvent.hpp
 *
 *  Created on: 20 Aug 2014
 *      Author: JAragon
 */

#ifndef MIXPANELEVENT_HPP_
#define MIXPANELEVENT_HPP_

#include <QObject>

#include "MixpanelPersistentIdentity.hpp"

class MixpanelEventPrivate;

/// \brief The MixpanelEvent class provides an interface for using Mixpanel Event Analytics features.
///
/// The recordEventMessage() signal is emitted whenever a event analytics message is tracked
///

class MixpanelEvent : public QObject
{
    Q_OBJECT
public:

   /// An enumeration for possible event errors
   enum TrackEventError
   {
      InvalidToken = 0,   ///< The token is invalid
      InvalidName,        ///< The event contained an invalid name
      InvalidIdentity,    ///< The event contained an invalid identity
      InvalidJson         ///< JSON conversation failed
   };

    MixpanelEvent(QObject* parent);
    virtual ~MixpanelEvent();

    void setDistinctId(const QString& distinctId);

    QString distinctId() const;

    MixpanelPersistentIdentity& persistentIdentity();

    void track(const QString& name, const QVariantMap& properties);
    QByteArray stdTrackEvent(const QString& name, const QVariantMap& properties) const;

private:
    bool eventHasErrors(const QString& eventName, const QVariantMap& properties);

signals:

    /// This signal is emitted when a valid event analytic message
    /// has to be recorded in order to be posted to the Mixpanel server.
    ///
    void recordEventMessage(const QByteArray& eventMessage);

    /// This signal is emitted when in invalid event message has
    /// failed to be recorded due an error.
    ///
    void trackError(MixpanelEvent::TrackEventError errorId, const QString eventName, const QVariantMap properties);

private:
    MixpanelEventPrivate * const d;
};

#endif /* MIXPANELEVENT_HPP_ */
