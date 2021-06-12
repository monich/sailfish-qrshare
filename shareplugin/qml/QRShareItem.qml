import QtQuick 2.0
import Sailfish.Silica 1.0

Item {
    property var shareAction

    width: parent.width
    height: isLandscape ? (Screen.width - Theme.itemSizeLarge - 2 * Theme.paddingLarge) : Screen.width

    property Item orientationComponent
    readonly property int orientation: orientationComponent ? orientationComponent.orientation : Orientation.None
    readonly property bool isLandscape: (orientation === Orientation.Landscape || orientation === Orientation.LandscapeInverted)

    onIsLandscapeChanged: console.log(isLandscape, height)

    Component.onCompleted: {
        var parentItem = parent
        while (parentItem) {
            if ('orientation' in parentItem) {
                orientationComponent = parentItem
                break
            }
            parentItem = parentItem.parent
        }
    }

    QRShare {
        anchors.fill: parent
        content: (shareAction && 'resources' in shareAction && shareAction.resources.length > 0) ? shareAction.resources[0] : undefined
        menuWidth: width - 2 * Theme.itemSizeSmall - 4 * Theme.paddingMedium
        hideInactiveMenu: true
    }
}
