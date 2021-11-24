# zenplayer

For use in your project add in your .pro file 

``` 
include('zenplayer/zenplayer.pri')
INCLUDEPATH += "zenplayer"
```

If your use widget, just create ZenPlayerWidget add in your widget ad call start, check widgetPlayer example

If your use qml, call 

```
ZenPlayer::qmlRegister();
```

for register qml items. Add import in your qml file

```
import ZenCat 1.0
```

and add component

```
Video {
    id: video
    anchors.fill: parent

    source: "./sample-mp4-file.mp4"

    onError: {
        console.debug("error " + error);
    }
}
```

```
Qt 5.15.2
ffmpeg 4.0
```
