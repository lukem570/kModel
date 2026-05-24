import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.kmodel

Kirigami.Page {
    id: page

    title: loader.source.toString().split("/").pop()
    padding: 0

    required property StlLoader loader

    VulkanViewport {
        anchors.fill: parent
        loader: page.loader
    }

    actions: [
        Kirigami.Action {
            text: "Model Info"
            icon.name: "document-properties"
            onTriggered: infoSheet.open()
        }
    ]

    Kirigami.OverlaySheet {
        id: infoSheet
        title: "Model Info"

        Kirigami.FormLayout {
            implicitWidth: Kirigami.Units.gridUnit * 20

            Kirigami.Heading {
                Kirigami.FormData.isSection: true
                text: page.loader.source.toString().split("/").pop()
                level: 3
            }

            Kirigami.Separator {
                Kirigami.FormData.isSection: true
            }

            QQC2.Label {
                Kirigami.FormData.label: "Triangles:"
                text: page.loader.triangleCount.toLocaleString()
            }
            QQC2.Label {
                Kirigami.FormData.label: "Min bounds:"
                text: "(%1, %2, %3)".arg(page.loader.minBounds.x.toFixed(2))
                                     .arg(page.loader.minBounds.y.toFixed(2))
                                     .arg(page.loader.minBounds.z.toFixed(2))
            }
            QQC2.Label {
                Kirigami.FormData.label: "Max bounds:"
                text: "(%1, %2, %3)".arg(page.loader.maxBounds.x.toFixed(2))
                                     .arg(page.loader.maxBounds.y.toFixed(2))
                                     .arg(page.loader.maxBounds.z.toFixed(2))
            }
        }
    }
}
