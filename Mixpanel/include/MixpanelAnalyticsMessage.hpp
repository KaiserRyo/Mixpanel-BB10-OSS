/*
 * AnalyticsMessage.hpp
 *
 *  Created on: 29 Aug 2014
 *      Author: JAragon
 */

#ifndef ANALYTICSMESSAGE_HPP_
#define ANALYTICSMESSAGE_HPP_

#include <QNetworkRequest>
#include <QSharedData>

class MixpanelAnalyticsMessagePrivate;

/// \brief The MixpanelAnalyticsMessage class represents an Mixpanel analytics message.
///

class MixpanelAnalyticsMessage
{
public:

    /// An enumeration for the type of analytics message.
    enum MessageType
    {
       Profile = 0, ///< Profile message type
       Event    ///< Event message type
    };

    MixpanelAnalyticsMessage();
    MixpanelAnalyticsMessage(const MessageType, const QByteArray&);
    MixpanelAnalyticsMessage(const MixpanelAnalyticsMessage &other);
    MixpanelAnalyticsMessage(const QVariantMap&);
    ~MixpanelAnalyticsMessage();
    MixpanelAnalyticsMessage& operator=(const MixpanelAnalyticsMessage &other);

    QNetworkRequest toNetworkRequest() const;
    QVariantMap toVariantMap() const;

private:
    QSharedDataPointer <MixpanelAnalyticsMessagePrivate> d;
};

#endif /* ANALYTICSMESSAGE_HPP_ */
