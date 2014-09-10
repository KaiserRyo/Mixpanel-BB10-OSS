/*
 * AnalyticsMessage.cpp
 *
 *  Created on: 29 Aug 2014
 *      Author: JAragon
 */

#include "../include/MixpanelAnalyticsMessage.hpp"

#include "../include/MixpanelConstants.hpp"

class MixpanelAnalyticsMessagePrivate : public QSharedData
{
public:
    MixpanelAnalyticsMessagePrivate();
    QByteArray content;
    MixpanelAnalyticsMessage::MessageType type;
};

/// Creates a MixpanelAnalyticsMessagePrivate object.

MixpanelAnalyticsMessagePrivate::MixpanelAnalyticsMessagePrivate()
    : content(QByteArray())
    , type(MixpanelAnalyticsMessage::Profile)
{

}

/// Creates a MixpanelAnalyticsMessage object.

MixpanelAnalyticsMessage::MixpanelAnalyticsMessage()
    : d(new MixpanelAnalyticsMessagePrivate)
{

}

/// Creates a MixpanelAnalyticsMessage object.
///
/// \param messageType Type of the analytic message
/// \param messageContent Raw data of the analytic message
///

MixpanelAnalyticsMessage::MixpanelAnalyticsMessage(const MessageType messageType, const QByteArray& messageContent)
    : d(new MixpanelAnalyticsMessagePrivate)
{
    d->type = messageType;
    d->content = messageContent;
}

/// Assigns \a other to this MixpanelAnalyticsMessage.

MixpanelAnalyticsMessage::MixpanelAnalyticsMessage(const MixpanelAnalyticsMessage &other)
    : d(other.d)
{
}

/// Creates a MixpanelAnalyticsMessage object from a QVaraintMap.
///
/// \param analyticsMap An analytic message cointained in a QVariantMap
///

MixpanelAnalyticsMessage::MixpanelAnalyticsMessage(const QVariantMap& analyticsMap)
    : d(new MixpanelAnalyticsMessagePrivate)
{
    d->type = (MixpanelAnalyticsMessage::MessageType) analyticsMap.value("type").toInt();
    d->content = analyticsMap.value("content").toByteArray();
}


/// Destructor, destroys the MixpanelConfiguration object.
///

MixpanelAnalyticsMessage::~MixpanelAnalyticsMessage()
{
}

/// Assigns \a other to this MixpanelConfiguration.

MixpanelAnalyticsMessage& MixpanelAnalyticsMessage::operator=(const MixpanelAnalyticsMessage &other)
{
    d = other.d;
    return *this;
}

/// Returns a QNetworkRequest containing the analytic message.
///
/// \retun network request
///

QNetworkRequest MixpanelAnalyticsMessage::toNetworkRequest() const
{
    QNetworkRequest networkRequest;

    QString request;
    if (d->type == MixpanelAnalyticsMessage::Event)
        request = g_urlTrackEvent;
    else
        request = g_urlEngageProfile;

    request += "?data=" + QString(d->content.toBase64());

    networkRequest.setUrl(QUrl(request));

    return networkRequest;
}

/// Returns a QVariantMap containing the analytic message.
///
/// \retun analyticsMap
///

QVariantMap MixpanelAnalyticsMessage::toVariantMap() const
{
    QVariantMap analyticsMap;

    analyticsMap.insert("type", QVariant::fromValue((int)d->type));
    analyticsMap.insert("content", QVariant::fromValue(d->content));

    return analyticsMap;
}
