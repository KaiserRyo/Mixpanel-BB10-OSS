/*
 * MixpanelConfiguration.cpp
 *
 *  Created on: 28 Aug 2014
 *      Author: JAragon
 */

#include "../include/MixpanelConfiguration.hpp"

#include "../include/MixpanelConstants.hpp"

class MixpanelConfigurationPrivate : public QSharedData
{
public:
    MixpanelConfigurationPrivate();
    MixpanelConfiguration::FlushMechanism flushPolicy;
    int flushInterval;
    bool thumbnailFlush;
    int messagesToFlush;
};

MixpanelConfigurationPrivate::MixpanelConfigurationPrivate()
    : flushPolicy(MixpanelConfiguration::Auto)
    , flushInterval(g_defaultFlushInterval)
    , thumbnailFlush(true)
    , messagesToFlush(MAX_SIZE_QUEUE)
{

}

/// Creates a MixpanelConfiguration object.

MixpanelConfiguration::MixpanelConfiguration()
    : d(new MixpanelConfigurationPrivate)
{

}

/// Creates a copy of \a other.

MixpanelConfiguration::MixpanelConfiguration(const MixpanelConfiguration &other)
    : d(other.d)
{
}

/// Destructor, destroys the MixpanelConfiguration object.
///

MixpanelConfiguration::~MixpanelConfiguration()
{
}

/// Assigns \a other to this MixpanelConfiguration.

MixpanelConfiguration& MixpanelConfiguration::operator=(const MixpanelConfiguration &other)
{
    d = other.d;
    return *this;
}

/// Sets the flush mechanism.
///
/// \param flushMechanism Mechanism that the will be use to flush the analytics messages. The default
/// value is Auto.
///

void MixpanelConfiguration::setFlushMechanism(const FlushMechanism flushMechanism)
{
    d->flushPolicy = flushMechanism;
}

/// Sets the flush interval.
///
/// \param interval Interval in miliseconds to send the analytics messages to Mixpanel. The default
/// value is 1800000 (30 minutes).
///

void MixpanelConfiguration::setFlushInterval(const int interval)
{
    if (interval > 0)
        d->flushInterval = interval;
}

/// Sets the thumbnail flush behaviour.
///
/// \param thumbnailFlush States whether the analytics messages are flush when the app is thumbnailed. The default
/// value is true.
///

void MixpanelConfiguration::setThumbnailFlush(const bool thumbnailFlush)
{
    d->thumbnailFlush = thumbnailFlush;
}

/// Sets the number of messages to flush.
///
/// \param numberOfMessages Number of messages that when reached the analytics messsages will be flushed. The default
/// value is MAX_SIZE_QUEUE = 20.
///

void MixpanelConfiguration::setMessagesToFlush(const int numberOfMessages)
{
    if (numberOfMessages > 0)
        d->messagesToFlush = numberOfMessages;
    else
        d->messagesToFlush = MAX_SIZE_QUEUE;
}

/// Returns the flush mechanism.
///
/// \return flush mechanism
///

MixpanelConfiguration::FlushMechanism MixpanelConfiguration::flushMechanism() const
{
    return d->flushPolicy;
}

/// Returns the flush interval.
///
/// \return flush interval (ms)
///

int MixpanelConfiguration::flushInterval() const
{
    return d->flushInterval;
}

/// Returns whether flush thumbnail is active.
///
/// \return flush thumbnail
///

bool MixpanelConfiguration::thumbnailFlush() const
{
    return d->thumbnailFlush;
}

/// Returns the number of messages to flush the analytics messages.
///
/// \return number of messages
///

int MixpanelConfiguration::messatesToFlush() const
{
    return d->messagesToFlush;
}
