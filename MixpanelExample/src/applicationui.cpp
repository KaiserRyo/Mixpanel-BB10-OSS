/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <QDateTime>

#include "Mixpanel.hpp";
#include "MixpanelConfiguration.hpp"

#include "qdebug.h"

using namespace bb::cascades;

static int numberContacts = 14;
/// Update token with your own token
static QString token = "";


ApplicationUI::ApplicationUI() :
        QObject()
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();



    MixpanelConfiguration config;
    config.setFlushInterval(60000);
    config.setFlushMechanism(MixpanelConfiguration::Auto);
    config.setMessagesToFlush(10);
    config.setThumbnailFlush(true);

    m_mixpanel = new Mixpanel(this, config);

    m_mixpanel->setToken(token);
    m_mixpanel->identify("");

    QVariantMap superProperties;
    superProperties.insert("Device Language", "en_GB");
    superProperties.insert("Number contact", numberContacts);
    m_mixpanel->registerSuperProperties(superProperties);

    superProperties.clear();
    superProperties.insert("First Login", QDateTime::currentDateTime().toString());
    m_mixpanel->registerSuperPropertiesOnce(superProperties);

    bool connectResult = false;
    Q_UNUSED(connectResult);

    connectResult = connect(&m_mixpanel->messageQueue(), SIGNAL(mixpanelMessagePosted(MixpanelMessageQueue::MixpanelPostMessageError,QVariantMap)), this, SLOT(mixpanelMessagePosted(MixpanelMessageQueue::MixpanelPostMessageError,QVariantMap)));
    Q_ASSERT(connectResult);

    connectResult = connect(&m_mixpanel->event(), SIGNAL(trackError(MixpanelEvent::TrackEventError,QString,QVariantMap)), this, SLOT(mixpanelTrackError(MixpanelEvent::TrackEventError,QString,QVariantMap)));
    Q_ASSERT(connectResult);

    connectResult = connect(&m_mixpanel->people(), SIGNAL(engageProfileError(MixpanelPeople::EngageProfileError,QString,QVariantMap)), this, SLOT(mixpanelEngageProfileError(MixpanelPeople::EngageProfileError,QString,QVariantMap)));
    Q_ASSERT(connectResult);

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    qml->setContextProperty("app", this);
    qml->setContextProperty("mixpanel", m_mixpanel);

    // Set created root object as the application scene
    Application::instance()->setScene(root);


    /// Set once
    m_mixpanel->setOnceProfileProperty("First launched", QVariant(Mixpanel::convertToMixpanelDateFormat(QDateTime::currentDateTimeUtc())));

}

void ApplicationUI::sendProfileUpdate()
{
    QVariantMap properties;
    properties.insert("$phone", "07987645159");
    properties.insert("$username", "lionelMessi");
    properties.insert("$first_name", "Leo");
    properties.insert("$last_name", "Messi");
    properties.insert("$email", "leomessi@gmail.com");
    m_mixpanel->setProfileProperties(properties);
}

void ApplicationUI::sendCoinsIncrement()
{
    m_mixpanel->incrementProfileProperty("Coins", (double)10);
}

void ApplicationUI::updateSuperProperties()
{
    numberContacts += 10;
    QVariantMap superProperty;
    superProperty.insert("Number contact", numberContacts);
    m_mixpanel->registerSuperProperties(superProperty);
}

void ApplicationUI::flushMixpanelMessages()
{
    m_mixpanel->flush();
}

void ApplicationUI::forceTrackEventError()
{
    m_mixpanel->trackEvent("", QVariantMap());
}

void ApplicationUI::forceEngageProfileError()
{
    m_mixpanel->setToken("");
    m_mixpanel->setProfileProperty("", QVariant());

    m_mixpanel->setToken("ead4bbe1941bbc94b95094b4ae3da953");
}



void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("MixpanelExample_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

void ApplicationUI::mixpanelMessagePosted(const MixpanelMessageQueue::MixpanelPostMessageError errorCode, const QVariantMap analyticMessage)
{
    qDebug() << errorCode <<  analyticMessage;
}

void ApplicationUI::mixpanelTrackError(MixpanelEvent::TrackEventError errorCode, const QString eventName, const QVariantMap eventProperties)
{
    switch (errorCode) {
        case MixpanelEvent::InvalidToken:
            qWarning() << "Event failed due to invalid token";
            break;
        case  MixpanelEvent::InvalidIdentity:
            qWarning() << "Event failed due to invalid identity";
            break;
        case  MixpanelEvent::InvalidJson:
            qWarning() << "Event failed due to invalid JSON content";
            break;
        case  MixpanelEvent::InvalidName:
            qWarning() << "Event failed due to an invalid event name";
            break;
        default:
            break;
    }
    qWarning() << "Mixpanel event " << eventName << " " <<  eventProperties;
}

void ApplicationUI::mixpanelEngageProfileError(const MixpanelPeople::EngageProfileError errorCode, const QString action, const QVariantMap properties)
{
    switch (errorCode) {
        case MixpanelPeople::InvalidToken:
            qWarning() << "Profile update failed due to invalid token";
            break;
        case  MixpanelPeople::InvalidIdentity:
            qWarning() << "Profile update failed due to invalid identity";
            break;
        case  MixpanelPeople::InvalidJson:
            qWarning() << "Profile update failed due to invalid JSON content";
            break;
        case  MixpanelPeople::InvalidIncrement:
            qWarning() << "Profile update failed due to invalid increment data";
            break;
        default:
            break;
    }
    qWarning() << "Action: " << action << "   Properties: " << properties;
}

