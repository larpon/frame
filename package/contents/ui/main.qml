/*
 *  Copyright 2015  Lars Pontoppidan <dev.larpon@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  2.010-1301, USA.
 */

import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.2

import org.kde.draganddrop 2.0 as DragDrop

import org.kde.plasma.plasmoid 2.0
import org.kde.plasma.core 2.0 as PlasmaCore
import org.kde.plasma.components 2.0 as PlasmaComponents
import org.kde.kquickcontrolsaddons 2.0

import org.kde.plasma.private.mediaframe 2.0

Item {
    id: main

    MediaFrame {
        id: items
        random: plasmoid.configuration.randomize
        useCustomCommand: plasmoid.configuration.useNextImageCommand
        customCommand: plasmoid.configuration.nextImageCommand
    }

    Plasmoid.preferredRepresentation: plasmoid.fullRepresentation

    Plasmoid.switchWidth: units.gridUnit * 5
    Plasmoid.switchHeight: units.gridUnit * 5

    Plasmoid.backgroundHints: plasmoid.configuration.useBackground ? PlasmaCore.Types.DefaultBackground : PlasmaCore.Types.NoBackground

    width: units.gridUnit * 20
    height: units.gridUnit * 13

    property string activeSource: ""
    property string transitionSource: ""

    property bool pause: overlayMouseArea.containsMouse

    readonly property int itemCount: (items.count + items.futureLength())
    readonly property bool hasItems: ((itemCount > 0) || (items.futureLength() > 0))
    readonly property bool isTransitioning: faderAnimation.running

    onActiveSourceChanged: {
        items.watch(activeSource)
    }

    onHasItemsChanged: {
        if(hasItems) {
            if(activeSource == "")
                nextItem()
        }
    }

    function loadPathList() {
        var list = plasmoid.configuration.pathList
        items.clear()
        for(var i in list) {
            var item = JSON.parse(list[i])
            items.add(item.path,true)
        }
    }

    Component.onCompleted: loadPathList()

    Connections {
        target: plasmoid.configuration
        onPathListChanged: loadPathList()
    }

    function addItem(item) {

        if(items.has(item.path)) {
            console.info(item.path,"already exists. Skipping...")
            return
        }

        plasmoid.configuration.pathList.push( JSON.stringify(item) )
    }

    function nextItem() {

        if(!hasItems) {
            console.warn("No items available")
            return
        }

        var active = activeSource

        // Only record history if we have more than one item
        if(itemCount > 1)
            items.pushHistory(active)

        if(items.futureLength() > 0) {
            setActiveSource(items.popFuture())
        } else {
            //setLoading()
            items.requestNext()
        }
    }

    function previousItem() {
        var active = activeSource
        items.pushFuture(active)
        var filePath = items.popHistory()
        setActiveSource(filePath)
    }

    Connections {
        target: items

        onItemChanged: {
            console.log("item",path,"changed")
            activeSource = ""
            setActiveSource(path)
        }

        onNextItemGotten: function (filePath, errorMessage) {
            if (errorMessage) {
                //unsetLoading()
                console.error("Error while getting next image",errorMessage)
            }
            else {
                setActiveSource(filePath)
                //unsetLoading()
            }
        }
    }

    Timer {
        id: nextTimer
        interval: (plasmoid.configuration.interval*1000)
        repeat: true
        running: hasItems && !pause
        onTriggered: nextItem()
    }

    Item {
        id: itemView
        anchors.fill: parent

        /*
        Video {
            id: video
            width : 800
            height : 600
            source: ""

            onStatusChanged: {
                if(status == Video.Loaded)
                    video.play()
            }
        }
        */

        Item {
            id: imageView
            visible: hasItems
            anchors.fill: parent

            Image {
                id: bufferImage


                anchors.fill: parent
                fillMode: plasmoid.configuration.fillMode

                opacity: 0

                cache: false
                source: transitionSource

                // TODO
                //asynchronous: true
                //autoTransform: true //new API in Qt 5.5, do not backport into Plasma 5.4.
            }

            Image {
                id: frontImage

                anchors.fill: parent
                fillMode: plasmoid.configuration.fillMode

                cache: false
                source: activeSource

                // TODO
                //asynchronous: true
                //autoTransform: true //new API in Qt 5.5, do not backport into Plasma 5.4.

                MouseArea {
                    anchors.fill: parent
                    onClicked: Qt.openUrlExternally(activeSource)
                    enabled: plasmoid.configuration.leftClickOpenImage
                }

            }
        }

        // BUG TODO fix the rendering of the drop shadow
        /*
        DropShadow {
            id: itemViewDropShadow
            anchors.fill: parent
            visible: imageView.visible && !plasmoid.configuration.useBackground

            radius: 8.0
            samples: 16
            color: "#80000000"
            source: frontImage
        }
        */

    }

    function setActiveSource(source) {
        if(itemCount > 1) { // Only do transition if we have more that one item
            transitionSource = source
            faderAnimation.restart()
        } else {
            transitionSource = source
            activeSource = source
        }
    }

    SequentialAnimation {
        id: faderAnimation

        ParallelAnimation {
            OpacityAnimator { target: frontImage; from: 1; to: 0; duration: 450 }
            OpacityAnimator { target: bufferImage; from: 0; to: 1; duration: 450 }
        }
        ScriptAction {
            script: {
                // Copy the transitionSource
                var ts = transitionSource
                activeSource = ts
                frontImage.opacity = 1
                transitionSource = ""
                bufferImage.opacity = 0
            }
        }
    }

    DragDrop.DropArea {
        id: dropArea
        anchors.fill: parent

        onDrop: {
            var mimeData = event.mimeData
            if (mimeData.hasUrls) {
                var urls = mimeData.urls
                for (var i = 0, j = urls.length; i < j; ++i) {
                    var url = urls[i]
                    var type = items.isDir(url) ? "folder" : "file"
                    var item = { "path":url, "type":type }
                    addItem(item)
                }
            }
            event.accept(Qt.CopyAction)
        }
    }

    Item {
        id: overlay

        anchors.fill: parent

        visible: hasItems
        opacity: overlayMouseArea.containsMouse ? 1 : 0

        Behavior on opacity {
            NumberAnimation {}
        }

        PlasmaComponents.Button {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            enabled: (items.historyLength() > 0) && !isTransitioning
            iconSource: "arrow-left"
            onClicked: {
                nextTimer.stop()
                previousItem()
            }
        }

        PlasmaComponents.Button {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            enabled: hasItems && !isTransitioning
            iconSource: "arrow-right"
            onClicked: {
                nextTimer.stop()
                nextItem()
            }
        }

        Row {
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: units.smallSpacing

            /*
            PlasmaComponents.Button {
                iconSource: "documentinfo"
                onClicked: {  }
            }
            */
            PlasmaComponents.Button {

                //text: activeSource.split("/").pop().slice(-25)
                iconSource: "document-preview"
                onClicked: Qt.openUrlExternally(main.activeSource)
                //tooltip: activeSource
            }
            /*
            PlasmaComponents.Button {
                iconSource: "trash-empty"
                onClicked: {  }
            }

            PlasmaComponents.Button {
                iconSource: "flag-black"
                onClicked: {  }
            }
            */
        }

        // BUG TODO Fix overlay so _all_ mouse events reach lower components
        MouseArea {
            id: overlayMouseArea

            anchors.fill: parent
            hoverEnabled: true

            propagateComposedEvents: true

            //onClicked: mouse.accepted = false;
            onPressed: mouse.accepted = false;
            //onReleased: mouse.accepted = false;
            onDoubleClicked: mouse.accepted = false;
            //onPositionChanged: mouse.accepted = false;
            //onPressAndHold: mouse.accepted = false;

        }

    }

    // Visualization of the count down
    // TODO Makes plasmashell suck CPU until the universe or the computer collapse in on itself
    /*
    Rectangle {
        id: progress

        visible: plasmoid.configuration.showCountdown && hasItems && itemCount > 1

        color: "transparent"

        implicitWidth: units.gridUnit
        implicitHeight: implicitWidth

        Rectangle {
            anchors.fill: parent

            opacity:  pause ? 0.1 : 0.5

            radius: width / 2
            color: "gray"

            Rectangle {
                id: innerRing
                anchors.fill: parent

                scale: 0

                radius: width / 2

                color: "lightblue"

                ScaleAnimator on scale {
                    running: nextTimer.running
                    loops: Animation.Infinite
                    from: 0;
                    to: 1;
                    duration: nextTimer.interval
                }

            }
        }

        PlasmaCore.IconItem {
            id: pauseIcon
            visible: pause
            anchors.fill: parent
            source: "media-playback-pause"
            colorGroup: PlasmaCore.ColorScope.colorGroup
        }
    }
    */

    PlasmaComponents.Button {

        anchors.centerIn: parent

        visible: !hasItems
        iconSource: "configure"
        text: "Configure plasmoid"
        onClicked: {
            plasmoid.action("configure").trigger();
        }
    }

}
