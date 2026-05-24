#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QIcon>

#include "stlloader.h"
#include "vulkanviewport.h"

int main(int argc, char *argv[])
{
    QQuickWindow::setGraphicsApi(QSGRendererInterface::Vulkan);

    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("kModel"));
    app.setOrganizationDomain(QStringLiteral("kde.org"));
    app.setDesktopFileName(QStringLiteral("org.kde.kmodel"));
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("applications-graphics")));

    QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));

    QQmlApplicationEngine engine;

    qmlRegisterType<StlLoader>("org.kde.kmodel", 1, 0, "StlLoader");
    qmlRegisterType<VulkanViewport>("org.kde.kmodel", 1, 0, "VulkanViewport");

    engine.loadFromModule("org.kde.kmodel", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    // If a file path was passed as argument, open it
    const QStringList args = app.arguments();
    if (args.size() > 1) {
        QUrl fileUrl = QUrl::fromLocalFile(args.last());
        QObject *root = engine.rootObjects().first();
        QObject *loader = root->property("loader").value<QObject *>();
        if (loader)
            loader->setProperty("source", fileUrl);
    }

    return app.exec();
}
