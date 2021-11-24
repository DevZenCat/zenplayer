#ifndef ZENPLAYERENGINE_H
#define ZENPLAYERENGINE_H

#include <QObject>
#include <QThread>
#include <QAudioOutput>

class AVFormatContext;
class AVCodecContext;
class AVCodec;
class AVFrame;
class AVPacket;
class SwrContext;

#include <QIODevice>
#include <QMutex>

class ZenPlayerEngine : public QObject
{
    Q_OBJECT

    public:
        explicit ZenPlayerEngine(QObject *parent = nullptr);

        void start(const QString &fileName);
        void stop();

    Q_SIGNALS:
        void error(const QString& error);
        void frameComplete(QImage *frame);

    private:
        void engine();
        bool init();

        void processError(const QString &errorString);

    private:
        bool _stopped;
        QThread _thread;
        QString _fileName;

        AVFormatContext *_formatContext;
        AVCodecContext *_videoCodecContext;
        AVCodec *_codec;

        AVCodecContext *_audioCodecContext;
        AVCodec *_audioCodec;

        AVFrame *_frame;
        AVFrame *_frameRGB;
        uint8_t *_outBuffer;

        AVPacket *_packet;
        AVPacket *_audioPacket;

        struct SwsContext *_swsContext;

        int _videoIndex;
        int _audioIndex;

        AVFrame *_audioFrame;

        SwrContext *_audioConvertContext;
        QAudioOutput *_audioOutput;
        QIODevice *_audioOutputDevice;

        AVFrame *_audioFramePCM;
        uint8_t *_audioBufferTemp;
        int _audioBufferSize;
};

#endif // ZENPLAYERENGINE_H
