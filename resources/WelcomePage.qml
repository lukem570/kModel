import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.kmodel

Kirigami.ScrollablePage {
    id: page

    title: "kModel"

    required property StlLoader loader

    Kirigami.PlaceholderMessage {
        anchors.centerIn: parent
        visible: loader.triangleCount === 0 && loader.error.length === 0
        width: parent.width - Kirigami.Units.gridUnit * 4
        text: "No model loaded"
        explanation: "Open an STL file to view model information"
        icon.name: "view-preview"

        helpfulAction: Kirigami.Action {
            text: "Open STL…"
            icon.name: "document-open"
            onTriggered: fileDialog.open()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        visible: loader.triangleCount > 0
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Heading {
            text: "Model Info"
            level: 2
            Layout.alignment: Qt.AlignHCenter
        }

        Kirigami.FormLayout {
            Layout.alignment: Qt.AlignHCenter

            QQC2.Label {
                Kirigami.FormData.label: "File:"
                text: loader.source.toString().split("/").pop()
            }
            QQC2.Label {
                Kirigami.FormData.label: "Triangles:"
                text: loader.triangleCount.toLocaleString()
            }
            QQC2.Label {
                Kirigami.FormData.label: "Min bounds:"
                text: "(%1, %2, %3)".arg(loader.minBounds.x.toFixed(2))
                                     .arg(loader.minBounds.y.toFixed(2))
                                     .arg(loader.minBounds.z.toFixed(2))
            }
            QQC2.Label {
                Kirigami.FormData.label: "Max bounds:"
                text: "(%1, %2, %3)".arg(loader.maxBounds.x.toFixed(2))
                                     .arg(loader.maxBounds.y.toFixed(2))
                                     .arg(loader.maxBounds.z.toFixed(2))
            }
        }

        Kirigami.InlineMessage {
            Layout.fillWidth: true
            text: "Vulkan 3D viewport coming soon — model data loaded successfully"
            type: Kirigami.MessageType.Positive
            visible: loader.triangleCount > 0
        }
    }

    Kirigami.InlineMessage {
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 4
        visible: loader.error.length > 0
        text: loader.error
        type: Kirigami.MessageType.Error
    }
}
