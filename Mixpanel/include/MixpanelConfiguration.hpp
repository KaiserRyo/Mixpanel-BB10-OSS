/*
 * MixpanelConfiguration.hpp
 *
 *  Created on: 28 Aug 2014
 *      Author: JAragon
 */

#ifndef MIXPANELCONFIGURATION_HPP_
#define MIXPANELCONFIGURATION_HPP_
#include "mixpanel_global.hpp"

#include <QSharedData>

class MixpanelConfigurationPrivate;


/// \brief The MixpanelConfiguration class holds configuration options.
///
/// It can be passed to Mixpanel to specify the options when sending data to
/// Mixpanel.
///
/// It is a container of all the settings, configuration required for
/// sending data to Mixpanel. E.g. flush mechanism, flush intevals,
/// analytics messages queue size
/// etc..
///

class MIXPANEL_EXPORT MixpanelConfiguration
{
public:

    /// An enumeration for type of the flush mechanism.
    /// The analytics messages can be sent manually by calling flush or automatically.
    enum FlushMechanism
    {
       Auto = 0, ///< Flush will be automatically call by the library (default)
       Manual    ///< The user will manually call flush
    };

    MixpanelConfiguration();
    MixpanelConfiguration(const MixpanelConfiguration &other);
    ~MixpanelConfiguration();
    MixpanelConfiguration& operator=(const MixpanelConfiguration &other);

    FlushMechanism flushMechanism() const;
    void setFlushMechanism(const FlushMechanism);

    int flushInterval() const;
    void setFlushInterval(const int);

    bool thumbnailFlush() const;
    void setThumbnailFlush(const bool);

    int messatesToFlush() const;
    void setMessagesToFlush(const int);


private:
    QSharedDataPointer <MixpanelConfigurationPrivate> d;
};

#endif /* MIXPANELCONFIGURATION_HPP_ */
