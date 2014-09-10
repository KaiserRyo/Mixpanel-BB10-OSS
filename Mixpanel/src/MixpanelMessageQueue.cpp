/*
 * AnalyticsMessages.cpp
 *
 *  Created on: 26 Aug 2014
 *      Author: JAragon
 */

#include "../include/MixpanelMessageQueue.hpp"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QSettings>
#include <QTimer>
#include <bb/Application>

#include "qdebug.h"
#include "../include/MixpanelConfiguration.hpp"
#include "../include/MixpanelConstants.hpp"
#include "../include/MixpanelAnalyticsMessage.hpp"


class MixpanelMessageQueuePrivate
{
public:
    QList<MixpanelAnalyticsMessage> messageQueue;
    QNetworkAccessManager* networkAccessManager;
    MixpanelConfiguration configuartion;
    QTimer* flushTimer;
    bool requestOngoing;
};

/// Creates a MixpanelMessageQueue object.

MixpanelMessageQueue::MixpanelMessageQueue(QObject* parent, MixpanelConfiguration config)
    : QObject(parent)
    , d(new MixpanelMessageQueuePrivate)
{
    d->networkAccessManager = new QNetworkAccessManager(this);
    d->flushTimer = NULL;
    d->configuartion = config;
    d->requestOngoing = false;

    initialise();
}

/// Destructor, destroys the MixpanelMessageQueue object.

MixpanelMessageQueue::~MixpanelMessageQueue()
{
    if (!d->messageQueue.isEmpty())
        saveMessageQueue();
    delete d;
}

/// Sets the configuration for the message queue and reinitialise with the new config
///
/// \param config Configuration for the message queue
///

void MixpanelMessageQueue::setConfiguration(const MixpanelConfiguration& config)
{
    d->configuartion = config;
    initialise();
}

/// Records a people analytics message into the message queue.
///
/// \param peopleMessage A raw analytic people message
///

void MixpanelMessageQueue::recordPeopleMessage(const QByteArray& peopleMessage)
{
    qDebug() << "Engage profile analytic message to record";
    recordAnalyticMessageAndProcessQueue(MixpanelAnalyticsMessage::Profile, peopleMessage);
}

/// Records a event analytics message into the message queue.
///
/// \param peopleMessage A raw analytic event message
///

void MixpanelMessageQueue::recordEventMessage(const QByteArray& eventMessage)
{
    qDebug() << "Event analytic message to record";
    recordAnalyticMessageAndProcessQueue(MixpanelAnalyticsMessage::Event, eventMessage);
}

/// Records a anaylic message in the queue and processs the queue to know whether or not
/// the messages need to be posted to the Mixpanel server.
///
/// \param type The analytic message type
/// \param content A raw analytic message
///

void MixpanelMessageQueue::recordAnalyticMessageAndProcessQueue(const MixpanelAnalyticsMessage::MessageType type, const QByteArray& content)
{
    MixpanelAnalyticsMessage analyticsMessage(type, content);
    d->messageQueue.push_back(analyticsMessage);

    qDebug() << "Analytic message queued";

    processMessageQueue();
}

/// Process the message queue and posts the first message to the Mixpanel servers.
///

void MixpanelMessageQueue::postToServer()
{
    qDebug() << "Posting pending anaylitics messages to Mixpanel server(" << d->messageQueue.size() << ")";
    if (d->messageQueue.isEmpty())
        return;

    if (!d->requestOngoing)
        postAnalyticsMessage(d->messageQueue.first());
}

/// Initialises the MixpanelMessageQueue
///

void MixpanelMessageQueue::initialise()
{

    bool connectResult = false;

    Q_UNUSED(connectResult);

    d->networkAccessManager->disconnect();
    this->disconnect();

    connectResult = connect(d->networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkRequestFinished(QNetworkReply*)));
    Q_ASSERT(connectResult);

    if (d->configuartion.flushMechanism() == MixpanelConfiguration::Auto)
    {
        setFlushTimerInterval(d->configuartion.flushInterval());
        setThumbnailFlush(d->configuartion.thumbnailFlush());
    }

    restoreMessageQueue();
    processMessageQueue();
}

/// Sets the flush interval to flush the message queue
///
/// \param flushInterval Flush interval in miliseconds
///

void MixpanelMessageQueue::setFlushTimerInterval(const int flushInterval)
{

    if (d->flushTimer)
        d->flushTimer->stop();
    else
        d->flushTimer = new QTimer(this);

    if (d->configuartion.flushInterval() > 0)
    {
        bool connectResult = false;
        Q_UNUSED(connectResult);

        d->flushTimer->disconnect();

        connectResult = connect(d->flushTimer, SIGNAL(timeout()), this, SLOT(flushIntervalTimeout()));
        Q_ASSERT(connectResult);

        d->flushTimer->setInterval(flushInterval);
        d->flushTimer->start();
    }
}

/// Sets the thumbnail flush capacity.
///
/// \param thumbnailFlushActive thubmnail flush active
///

void MixpanelMessageQueue::setThumbnailFlush(const bool thumbnailFlushActive)
{
    bb::Application::instance()->disconnect(SIGNAL(thumbnail()));

    if (thumbnailFlushActive)
    {
        bool connectResult = false;
        Q_UNUSED(connectResult);

        connectResult = connect(bb::Application::instance(), SIGNAL(thumbnail()), this, SLOT(appThumbnail()));
        Q_ASSERT(connectResult);
    }
}

/// Posts a web request containing an analytic message
///
/// \param analyticsMessage The anaylict message to be posted
///

void MixpanelMessageQueue::postAnalyticsMessage(const MixpanelAnalyticsMessage& analyticsMessage)
{
    qDebug() << "Posting analytics message";
    d->networkAccessManager->get(analyticsMessage.toNetworkRequest());
    d->requestOngoing = true;
}

/// Processes the message queue to check whether messsges need to be posted to the Mixpanel servers
///
/// \note If the size of the queue is greater than MAX_SIZE_QUEUE the messages will be posted
/// independently of the flush mechanism selected
///

void MixpanelMessageQueue::processMessageQueue()
{
    qDebug() << "Processing message queue(" << d->messageQueue.size() << ")";
    if ((d->configuartion.flushMechanism() == MixpanelConfiguration::Auto) && (d->messageQueue.size() >= d->configuartion.messatesToFlush()))
    {
        qDebug() << "Message queue size(" << d->messageQueue.size() << ") -> post pending messages to Mixpanel server";
        postToServer();
    } else {
        if (d->messageQueue.size() >= MAX_SIZE_QUEUE)
        {
            qDebug() << "Message queue size(" << d->messageQueue.size() << ") limit reached -> post pending messages to Mixpanel server";
            postToServer();
        }
    }
}

/// Slot called when the flush interval times out
/// \note It posts the pending messages to the Mixpanel server
///

void MixpanelMessageQueue::flushIntervalTimeout()
{
    qDebug() << "Interval timeout -> post pending messages to Mixpanel server";
    postToServer();
}

/// Slot called when the app thumbanail
///
/// \note It posts the pending messages to the Mixpanel server
///

void MixpanelMessageQueue::appThumbnail()
{
    qDebug() << "App thumbnail -> post pending messages to Mixpanel server";
    postToServer();
}

/// Saves the current message queue into QSettings
///

void MixpanelMessageQueue::saveMessageQueue()
{
    qDebug() << "Saving pending analytics messages (" << d->messageQueue.size() << ")";
    QSettings settings(g_organizationName);

    QVariantList analyticsMessages;
    Q_FOREACH(MixpanelAnalyticsMessage analyticsMessage, d->messageQueue)
    {
        analyticsMessages.push_back(analyticsMessage.toVariantMap());
    }
    settings.setValue(g_analyticsMessagesKey, analyticsMessages);
}

/// Restores the latest message queue
///

void MixpanelMessageQueue::restoreMessageQueue()
{

    QSettings settings(g_organizationName);

    QVariantList analyticsMessages = settings.value(g_analyticsMessagesKey).toList();

    Q_FOREACH(QVariant analytics, analyticsMessages)
    {
        MixpanelAnalyticsMessage analyticsMessage(analytics.toMap());
        d->messageQueue.push_back(analyticsMessage);
    }

    settings.remove(g_analyticsMessagesKey);

    qDebug() << "Pending Analytics Messages restored from last session (" << d->messageQueue.size() << ")";

}

/// Slot called when a network request to Mixpanel server finishes
///
/// \note If the request fails due a network error the analytic message will be
///  sent next time that the message queue is processed
///
///  If Mixpanel server does not accept the message the message will be
///  deleted adn the next message in the queue will be processed
///
///  If Mixpanel server accepts the message the next message the next
///  message in the queue will be processed
///

void MixpanelMessageQueue::networkRequestFinished(QNetworkReply* reply)
{
    d->requestOngoing = false;

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray mixpanelResponse = reply->readAll();
        if (mixpanelResponse.toInt() == MixpanelError)
        {
            qDebug() << "Error due to incorrect Analytic Message sent to Mixpanel server";
            emit mixpanelMessagePosted(MixpanelError, d->messageQueue.first().toVariantMap());
        } else {
            qDebug() << "Analytic Message sent successfully to Mixpanel server";
            emit mixpanelMessagePosted(NoError, d->messageQueue.first().toVariantMap());
        }
        d->messageQueue.pop_front();
        postToServer();
    } else {
        qWarning() << "Network request error (" << reply->error() << "): " << reply->errorString();
        emit mixpanelMessagePosted(NetworkError, d->messageQueue.first().toVariantMap());
    }
}

