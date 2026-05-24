#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QIcon>

#include "stlloader.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("kModel"));
    app.setOrganizationDomain(QStringLiteral("kde.org"));
    app.setDesktopFileName(QStringLiteral("org.kde.kmodel"));
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("applications-graphics")));

    QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));

    QQmlApplicationEngine engine;

    qmlRegisterType<StlLoader>("org.kde.kmodel", 1, 0, "StlLoader");

    engine.loadFromModule("org.kde.kmodel", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
