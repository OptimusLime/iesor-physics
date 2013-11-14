#include "QtFabric/view/qtfabric.h"
#include <QtWebKit/qwebview.h>
#include <QtGui/QApplication>
#include <IESoR/Simulation/iesorWorld.h>


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
    QUrl url;
    if (argc > 1)
        url = QUrl(argv[1]);
    else
        url = QUrl("http://www.google.com/ncr");
    
	QtFabric *browser = new QtFabric();
    #if defined Q_OS_SYMBIAN || defined Q_WS_HILDON || defined Q_WS_MAEMO_5 || defined Q_WS_SIMULATOR
        browser->showMaximized();
    #else
        browser->show();
    #endif

		

    return app.exec();
}
