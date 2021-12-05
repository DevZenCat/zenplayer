import QtQuick 2.15
import QtQuick.Window 2.15

import ZenCat 1.0

Window {
    width: 640
    height: 480

    visible: true

    property bool paused: true;

    Video {
        id: video
        anchors.fill: parent

        source: "./sample-mp4-file.mp4"

        onError: {
            console.debug("error " + error);
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            if (paused) {
                paused = false;
                video.play();
            }else {
                paused = true;
                video.pause();
            }
        }
    }
}
