#ifndef ZENPLAYERENGINE_H
#define ZENPLAYERENGINE_H

#include <QObject>
#include <QThread>
#include <QAudioOutput>
#include <QIODevice>
#include <QMutex>

//#include "libavcodec/avcodec.h"

class AVFormatContext;
class AVCodecContext;
class AVCodec;
class AVFrame;
class AVPacket;
class SwrContext;
class AVCodecParameters;

class ZenPlayerEngine : public QObject
{
    Q_OBJECT

    public:
        explicit ZenPlayerEngine(QObject *parent = nullptr);

        bool open(const QString &fileName);
        void play();
        void pause();
        void stop();

        void seek(uint64_t second);

    Q_SIGNALS:
        void error(const QString& error);
        void frameComplete(QImage *frame);

    private:
        void engine();
        bool init();
        bool openVideoStream();
        bool openAudioStream();

        void getFirstVideoFrame();

        void processError(const QString &errorString);

    private:
        bool _stopped;
        bool _pause;
        QThread _thread;
        QString _fileName;

        AVFormatContext *_formatContext;
        AVCodecContext *_videoCodecContext;
        AVCodec *_videoCodec;
        AVCodecParameters *_videoCodecParameters;

        AVCodecContext *_audioCodecContext;
        AVCodec *_audioCodec;
        AVCodecParameters *_audioCodecParameters;

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

        std::condition_variable _pauseCondition;
        std::mutex _pauseMutex;
        QElapsedTimer _elapsedTimer;

        uint64_t _timeline;
        uint64_t _pauseTimeLine;
};

#endif // ZENPLAYERENGINE_H
