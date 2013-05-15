#include "hellocascadesapp.h"
#include "tealiumtagger.h"

#include <bb/cascades/QmlDocument>
#include <bb/cascades/Page>

using namespace bb::cascades;

HelloCascadesApp::HelloCascadesApp()
{
    QmlDocument *qml = QmlDocument::create("asset:///hellocascades.qml");

    if (!qml->hasErrors()) {

        Page *appPage = qml->createRootObject<Page>();

        if (appPage) {
            Application::instance()->setScene(appPage);
        }

        TealiumTagger *TealiumTaggerObj = new TealiumTagger(bb::cascades::Application::instance(), "tealium", "mike", "prod");
        QMap<QString,QString> m;
        m.insert("test","1");
        TealiumTaggerObj->trackScreenViewed("Home   1", m);
        m.insert("test","2");
        TealiumTaggerObj->trackScreenViewed("Home   2", m);
        m.insert("test","3");
        TealiumTaggerObj->trackScreenViewed("Home   3", m);
    }
}
