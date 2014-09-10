#include "MixpanelModuleTest.hpp"

#include <QtTest/QtTest>
#include <bb/data/JsonDataAccess>
#include <bb/device/HardwareInfo>
#include <bb/ApplicationInfo>
#include <bb/platform/PlatformInfo>

#include "MixpanelPeople.hpp"
#include "MixpanelEvent.hpp"
#include "MixpanelPersistentIdentity.hpp"
#include "MixpanelConfiguration.hpp"
#include "MixpanelMessageQueue.hpp"

using namespace bb::data;

static MixpanelPeople* mixPeople = new MixpanelPeople(NULL);
static MixpanelEvent* mixEvent = new MixpanelEvent(NULL);
static MixpanelPersistentIdentity persistentIdentity;
static MixpanelConfiguration mixpanelConfig;



void MixpanelModuleTest::testPersistentProperties()
{
    bb::device::HardwareInfo hardwareInfo;
    bb::ApplicationInfo appInfo;
    bb::platform::PlatformInfo platformInfo;

    persistentIdentity.readIdentities();

    QVariantMap properties = persistentIdentity.referrerProperties();
    QCOMPARE(properties["$os"].toString(), QString("BB10"));
    QCOMPARE(properties["App version"].toString(), appInfo.version());
    QCOMPARE(properties["BB10 OS version"].toString(), platformInfo.osVersion());
    QCOMPARE(properties["Device name"].toString(), hardwareInfo.deviceName());
    QCOMPARE(properties["Model"].toString(), hardwareInfo.modelName());
    QCOMPARE(properties["PIN"].toString(), hardwareInfo.pin());

    persistentIdentity.setToken("36ada5b10da39a1347559321baf13063");
    QCOMPARE(QString("36ada5b10da39a1347559321baf13063"), persistentIdentity.token());

}


void MixpanelModuleTest::testRegisterSuperProperties()
{
    QVariantMap expectedSuperProperties;
    expectedSuperProperties.insert("Device Language", "English");
    expectedSuperProperties.insert("Number contacts", 7);
    persistentIdentity.registerSuperProperties(expectedSuperProperties);

    QVariantMap actualSuperProperties = persistentIdentity.eventSuperProperties();

    QCOMPARE(actualSuperProperties["Device Language"].toString(), expectedSuperProperties["Device Language"].toString());
    QCOMPARE(actualSuperProperties["Number contacts"].toInt(), expectedSuperProperties["Number contacts"].toInt());

    QVariantMap propertyUpdated;
    propertyUpdated.insert("Number contacts", 14);
    persistentIdentity.registerSuperProperties(propertyUpdated);

    actualSuperProperties.clear();
    actualSuperProperties = persistentIdentity.eventSuperProperties();

    QCOMPARE(actualSuperProperties.size(), 2);
    QCOMPARE(actualSuperProperties["Number contacts"].toInt(), 14);
}



void MixpanelModuleTest::testUnregisterSuperProperty()
{
    persistentIdentity.unregisterSuperProperty("Device Language");

    QVariantMap actualSuperProperties = persistentIdentity.eventSuperProperties();

    QCOMPARE(actualSuperProperties.contains("Device Language"), false);
    QCOMPARE(actualSuperProperties.size(), 1);
}


void MixpanelModuleTest::testRegisterSuperPropertiesOnce()
{
    QVariantMap superProperty;
    superProperty.insert("First login date", "2013-04-01T13:20:00");
    persistentIdentity.registerSuperPropertiesOnce(superProperty);

    QVariantMap actualSuperProperties = persistentIdentity.eventSuperProperties();

    QCOMPARE(actualSuperProperties["First login date"].toString(), QString("2013-04-01T13:20:00"));

    superProperty["First login date"] = "2011-24-11T13:22:13";

    persistentIdentity.registerSuperPropertiesOnce(superProperty);

    actualSuperProperties.clear();
    actualSuperProperties = persistentIdentity.eventSuperProperties();

    QCOMPARE(actualSuperProperties["First login date"].toString(), QString("2013-04-01T13:20:00"));

}

void MixpanelModuleTest::testLoadSuperProperties()
{
    persistentIdentity.loadPersistentData();

    QVariantMap actualSuperProperties = persistentIdentity.eventSuperProperties();

    QCOMPARE(actualSuperProperties.size(), 2);
    QCOMPARE(actualSuperProperties["Number contacts"].toInt(), 14);
    QCOMPARE(actualSuperProperties["First login date"].toString(), QString("2013-04-01T13:20:00"));
}

void MixpanelModuleTest::testUnregisterAllSuperProperties()
{
    persistentIdentity.clearSuperProperties();

    QCOMPARE(persistentIdentity.eventSuperProperties().isEmpty(), true);
}


void MixpanelModuleTest::testInitDistinctIdEvent()
{
    mixEvent->persistentIdentity() = persistentIdentity;
    mixEvent->setDistinctId("13793");
    QCOMPARE(QString("13793"), mixEvent->distinctId());
    QCOMPARE(QString("13793"), persistentIdentity.eventDistinctId());
}



void MixpanelModuleTest::testInitDisctinctIdPeople()
{
    mixPeople->persistentIdentity() = persistentIdentity;

    mixPeople->setDistinctId("13793");

    QCOMPARE(QString("13793"), mixPeople->distinctId());
    QCOMPARE(QString("13793"), persistentIdentity.peopleDistinctId());

}

void MixpanelModuleTest::testSetPeopleProperties()
{
    QVariantMap properties;
    properties.insert("Address", "1313 Mockingbird Lane");
    properties.insert("Birthday", "1948-01-01");

    QByteArray messageJson = mixPeople->stdPeopleMessage("$set", properties);

    JsonDataAccess dataAccess;
    QVariantMap jsonData = dataAccess.loadFromBuffer(messageJson).toMap();


    QCOMPARE(jsonData["$set"].toMap().value("Address").toString(), QString("1313 Mockingbird Lane"));
    QCOMPARE(jsonData["$set"].toMap().value("Birthday").toString(), QString("1948-01-01"));
}

void MixpanelModuleTest::testSetPeopleProperty()
{
    QVariantMap property;
    property.insert("Address", "1313 Mockingbird Lane");

    QByteArray messageJson = mixPeople->stdPeopleMessage("$set", property);

    JsonDataAccess dataAccess;
    QVariantMap jsonData = dataAccess.loadFromBuffer(messageJson).toMap();

    QCOMPARE(jsonData["$set"].toMap().value("Address").toString(), QString("1313 Mockingbird Lane"));
}



void MixpanelModuleTest::testTrackSingleEvent()
{

    QByteArray messageJson = mixEvent->stdTrackEvent("Signed Up", QVariantMap());

    JsonDataAccess dataAccess;
    QVariantMap jsonData = dataAccess.loadFromBuffer(messageJson).toMap();

    QCOMPARE(jsonData.value("event").toString(), QString("Signed Up"));
    QCOMPARE(jsonData["properties"].toMap().value("distinct_id").toString(), QString("13793"));
    QCOMPARE(jsonData["properties"].toMap().value("token").toString(), QString("36ada5b10da39a1347559321baf13063"));

}

void MixpanelModuleTest::testTrackEventProperties()
{
    QVariantMap eventProperties;

    eventProperties.insert("Level Number", QVariant(9));
    eventProperties.insert("ip", "203.0.113.9");

    QByteArray messageJson = mixEvent->stdTrackEvent("Level Complete", eventProperties);

    JsonDataAccess dataAccess;
    QVariantMap jsonData = dataAccess.loadFromBuffer(messageJson).toMap();

    QCOMPARE(jsonData.value("event").toString(), QString("Level Complete"));
    QCOMPARE(jsonData["properties"].toMap().value("distinct_id").toString(), QString("13793"));
    QCOMPARE(jsonData["properties"].toMap().value("token").toString(), QString("36ada5b10da39a1347559321baf13063"));
    QCOMPARE(jsonData["properties"].toMap().value("ip").toString(), QString("203.0.113.9"));
    QCOMPARE(jsonData["properties"].toMap().value("Level Number").toInt(), 9);

}

void MixpanelModuleTest::testMixpanelConfiguration()
{
    mixpanelConfig.setFlushInterval(60);
    mixpanelConfig.setFlushMechanism(MixpanelConfiguration::Auto);
    mixpanelConfig.setMessagesToFlush(20);

    QCOMPARE(mixpanelConfig.flushInterval(), 60);
    QCOMPARE(mixpanelConfig.flushMechanism(), MixpanelConfiguration::Auto);
    QCOMPARE(mixpanelConfig.messatesToFlush(), 20);
}







