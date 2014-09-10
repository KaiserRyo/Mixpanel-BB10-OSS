#ifndef MIXPANELMODULETEST_HPP
#define MIXPANELMODULETEST_HPP

#include <QObject>

class MixpanelModuleTest : public QObject
{
    Q_OBJECT

private slots:
    void testPersistentProperties();
    void testRegisterSuperProperties();
    void testUnregisterSuperProperty();
    void testRegisterSuperPropertiesOnce();
    void testLoadSuperProperties();
    void testUnregisterAllSuperProperties();
    void testInitDistinctIdEvent();
    void testInitDisctinctIdPeople();
    void testSetPeopleProperties();
    void testSetPeopleProperty();
    void testTrackSingleEvent();
    void testTrackEventProperties();
    void testMixpanelConfiguration();

};

#endif
