import QtQuick 2.15
import QtQuick.Window 2.15

import ZenCat 1.0

Window {
    width: 640
    height: 480

    visible: true

    Video {
        id: video
        anchors.fill: parent

        source: "./sample-mp4-file.mp4"

        onError: {
            console.debug("error " + error);
        }
    }

    Component.onCompleted: {
        video.start();
    }
}
