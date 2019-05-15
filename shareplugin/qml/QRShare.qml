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

    property string lastSavedQrCode

    readonly property string qrCode: generator.qrcode
    readonly property bool haveQrCode: qrCode.length > 0
    readonly property bool needPullDownMenu: haveQrCode && qrCode !== lastSavedQrCode

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            id: menu

            visible: page.needPullDownMenu

            property string savedQrCode

            onActiveChanged: {
                if (!active && savedQrCode) {
                    // Don't save the same code twice
                    page.lastSavedQrCode = savedQrCode
                    savedQrCode = ""
                }
            }

            MenuItem {
                //: Pulley menu item
                //% "Save to Gallery"
                text: qsTrId("qrshare-menu-save_to_gallery")
                onClicked: {
                    if (QRCodeUtils.saveToGallery(page.qrCode, "QRShare", "qrcode")) {
                        menu.savedQrCode = page.qrCode
                    }
                }
            }
        }

        Rectangle {
            id: qrcodeRect

            color: "white"
            visible: page.haveQrCode
            radius: Theme.paddingMedium
            anchors.centerIn: parent
            width: qrcodeImage.width + 2 * Theme.horizontalPageMargin
            height: qrcodeImage.height + 2 * Theme.horizontalPageMargin

            readonly property int margins: Math.round((Math.min(Screen.width, Screen.height) - Math.max(width, height))/2)

            Image {
                id: qrcodeImage

                asynchronous: true
                anchors.centerIn: parent
                source: page.haveQrCode ? "image://qrcode/" + page.qrCode : ""
                width: sourceSize.width * n
                height: sourceSize.height * n
                smooth: false

                readonly property int maxDisplaySize: Math.min(Screen.width, Screen.height) - 4 * Theme.horizontalPageMargin
                readonly property int maxSourceSize: Math.max(sourceSize.width, sourceSize.height)
                readonly property int n: maxSourceSize ? Math.floor(maxDisplaySize/maxSourceSize) : 0
            }
        }

        ViewPlaceholder {
            // No need to animate opacity
            Behavior on opacity { enabled: false }
            enabled: !page.haveQrCode && !startTimer.running && !generator.running
            text: generator.running ? "" : generator.text ?
                //: Placeholder text
                //% "Text is too long for QR code."
                qsTrId("qrshare-placeholder-text_too_long") :
                //: Placeholder text
                //% "Nothing to share."
                qsTrId("qrshare-placeholder-no_text")
        }
    }

    QrCodeGenerator {
        id: generator

        text: content.data
    }

    Timer {
        id: startTimer

        running: true
        interval: 500
    }
}
