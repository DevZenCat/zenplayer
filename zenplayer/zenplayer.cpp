#include "zenplayer.h"
#include "zenplayerqmlcomponentvideo.h"

ZenPlayer::ZenPlayer(QObject *parent)
    : QObject{parent}
{

}

void ZenPlayer::qmlRegister()
{
    qmlRegisterType<ZenPlayerQmlComponentVideo>("ZenCat", 1, 0, "Video");
}
