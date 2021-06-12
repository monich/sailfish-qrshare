import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    property url source
    property alias content: qrShare.content
    property string methodId
    property string displayName
    property int accountId
    property string accountName
    property var shareEndDestination

    PageHeader { id: header }

    QRShare {
        id: qrShare

        anchors.fill: parent
        verticalMargins: header.height
    }
}
