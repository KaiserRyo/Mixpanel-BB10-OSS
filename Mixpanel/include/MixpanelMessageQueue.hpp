/*
 * AnalyticsMessages.hpp
 *
 *  Created on: 26 Aug 2014
 *      Author: JAragon
 */

#ifndef ANALYTICSMESSAGES_HPP_
#define ANALYTICSMESSAGES_HPP_

#include "../include/MixpanelConfiguration.hpp"

#include <QObject>
#include <QNetworkReply>

#include "MixpanelAnalyticsMessage.hpp"

class MixpanelMessageQueuePrivate;

/// \brief The MixpanelMessageQueue class manages communication of analytic messages to the servers.
///
/// \note The MixpanelMessageQueue should be configure (setConfiguration(config)) before start sending
///  data to the Mixpanel servers. See MixpanelConfiguration for more information.
///

class MixpanelMessageQueue: public QObject
{
    Q_OBJECT
public:

    enum MixpanelPostMessageError
    {
       MixpanelError = 0,  ///< Error due to incorrect data sent to Mixpanel
       NetworkError,       ///< Error due to a network error
       NoError             ///< No error.
    };

    MixpanelMessageQueue(QObject *parent = 0, MixpanelConfiguration config = MixpanelConfiguration());
    virtual ~MixpanelMessageQueue();

    void setConfiguration(const MixpanelConfiguration& config);

    void saveMessageQueue();
    void restoreMessageQueue();

signals:

    /// This signal is emitted when a mixpanel message has been posted to
    /// the Mixpanel server.
    ///
    void mixpanelMessagePosted(const MixpanelMessageQueue::MixpanelPostMessageError errorId, const QVariantMap analyticMessage);

public slots:
    void recordPeopleMessage(const QByteArray& peopleMessage);
    void recordEventMessage(const QByteArray& eventMessage);
    void postToServer();


private slots:
    void flushIntervalTimeout();
    void appThumbnail();
    void networkRequestFinished(QNetworkReply* reply);


private:
    void initialise();
    void setFlushTimerInterval(const int flushInterval);
    void setThumbnailFlush(const bool);
    void recordAnalyticMessageAndProcessQueue(const MixpanelAnalyticsMessage::MessageType, const QByteArray&);
    void processMessageQueue();
    void postAnalyticsMessage(const MixpanelAnalyticsMessage&);



private:
    MixpanelMessageQueuePrivate * const d;
};

#endif /* ANALYTICSMESSAGES_HPP_ */
