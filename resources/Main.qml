import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import Qt.labs.platform as Platform

import org.kde.kirigami as Kirigami

import org.kde.kmodel

Kirigami.ApplicationWindow {
    id: root

    title: "kModel"
    width: 900
    height: 650

    property StlLoader loader: StlLoader {}

    globalDrawer: Kirigami.GlobalDrawer {
        title: "kModel"
        titleIcon: "applications-graphics"
        isMenu: true
        actions: [
            Kirigami.Action {
                text: "Open STL…"
                icon.name: "document-open"
                onTriggered: fileDialog.open()
            },
            Kirigami.Action {
                text: "Quit"
                icon.name: "application-exit"
                onTriggered: Qt.quit()
            }
        ]
    }

    Platform.FileDialog {
        id: fileDialog
        title: "Open STL File"
        nameFilters: ["STL files (*.stl)", "All files (*)"]
        onAccepted: root.loader.source = file
    }

    pageStack.initialPage: WelcomePage {
        loader: root.loader
    }
}
