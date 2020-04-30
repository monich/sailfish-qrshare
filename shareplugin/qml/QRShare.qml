import QtQuick 2.0
import Sailfish.Silica 1.0
import com.monich.qrshare 1.0

Page {
    id: page

    property url source
    property variant content: ({})
    property string methodId
    property string displayName
    property int accountId
    property string accountName
    property var shareEndDestination

    readonly property var generators: [ qrCodeGenerator, aztecCodeGenerator ]
    readonly property string text: content ?
        ('data' in content) ? content.data :
        ('status' in content) ? content.status : "" : ""

    QrCodeGenerator {
        id: qrCodeGenerator

        text: page.text
        readonly property string name: "QR code"
        readonly property string baseName: "qr-code"
        //: Placeholder text
        //% "Text is too long for QR code."
        readonly property string textTooLong: qsTrId("qrshare-placeholder-qrcode-too_long")
    }

    AztecCodeGenerator {
        id: aztecCodeGenerator

        text: page.text
        readonly property string name: "Aztec"
        readonly property string baseName: "aztec-code"
        //: Placeholder text
        //% "Text is too long for Aztec code."
        readonly property string textTooLong: qsTrId("qrshare-placeholder-aztec-too_long")
    }

    Timer {
        id: startTimer

        running: true
        interval: 500
    }

    Component.onCompleted: view.currentIndex = 0

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            id: menu

            visible: view.currentItem && view.currentItem.needPullDownMenu

            property string savedCode

            onActiveChanged: {
                if (!active && savedCode) {
                    // Don't save the same code twice
                    if (view.currentItem) view.currentItem.lastSavedCode = savedCode
                    savedCode = ""
                }
            }

            MenuItem {
                //: Pulley menu item
                //% "Save to Gallery"
                text: qsTrId("qrshare-menu-save_to_gallery")
                onClicked: {
                    var item = view.currentItem
                    var code = item.code
                    if (QRCodeUtils.saveToGallery(code, "QRShare", item.baseName, Math.min(item.scaleFactor, 5))) {
                        menu.savedCode = code
                    }
                }
            }
        }

        PageHeader { id: header }

        SilicaListView {
            id: view

            anchors.centerIn: parent
            height: Math.min(parent.width, parent.height - 2 * header.height)
            width: parent.width
            orientation: ListView.Horizontal
            snapMode: ListView.SnapOneItem
            highlightRangeMode: ListView.StrictlyEnforceRange
            clip: true
            model: generators

            delegate: Item {
                id: delegate

                height: view.height
                width: view.width

                property string lastSavedCode
                readonly property var generator: modelData
                readonly property string code: generator.code
                readonly property string baseName: generator.baseName
                readonly property bool haveCode: code.length > 0
                readonly property bool needPullDownMenu: haveCode && code !== lastSavedCode
                readonly property alias scaleFactor: image.n

                Rectangle {
                    color: "white"
                    visible: delegate.haveCode
                    radius: Theme.paddingMedium
                    anchors.centerIn: parent
                    width: image.width + 2 * Theme.horizontalPageMargin
                    height: image.height + 2 * Theme.horizontalPageMargin

                    Image {
                        id: image

                        asynchronous: true
                        anchors.centerIn: parent
                        source: delegate.haveCode ? "image://qrcode/" + delegate.code : ""
                        width: sourceSize.width * n
                        height: sourceSize.height * n
                        smooth: false

                        readonly property int maxDisplaySize: Math.min(delegate.width - 2 * Theme.horizontalPageMargin, delegate.height) - 4 * Theme.horizontalPageMargin
                        readonly property int maxSourceSize: Math.max(sourceSize.width, sourceSize.height)
                        readonly property int n: maxSourceSize ? Math.floor(maxDisplaySize/maxSourceSize) : 0
                    }
                }

                Loader {
                    readonly property bool oops: page.text.length && !delegate.haveCode && !startTimer.running && !generator.running
                    anchors.fill: parent
                    opacity: oops ? 1 : 0
                    active: opacity > 0
                    Behavior on opacity { FadeAnimation { } }
                    sourceComponent: Item {
                        Image {
                            source: "image://qrshare/" + Qt.resolvedUrl("unhappy.svg") + "?" + Theme.highlightColor
                            height: Math.max(Theme.itemSizeMedium, Math.min(Theme.itemSizeHuge, label.y - Theme.paddingLarge))
                            sourceSize.height: height
                            visible: (y >= 0)
                            anchors {
                                horizontalCenter: parent.horizontalCenter
                                bottom: label.top
                                bottomMargin: Theme.paddingLarge
                            }
                        }
                        InfoLabel {
                            id: label

                            text: generator.textTooLong
                            anchors.verticalCenter: parent.verticalCenter
                        }
                    }
                }
            }
        }

        ViewPlaceholder {
            // No need to animate opacity
            Behavior on opacity { enabled: false }
            enabled: !page.text.length
            //: Placeholder text
            //% "Nothing to share."
            text: qsTrId("qrshare-placeholder-no_text")
        }
    }
}
