#include "zenplayerengine.h"

#include <QDebug>
#include <QImage>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QFile>

extern "C"
{
    #include "libavdevice/avdevice.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include "libavcodec/avcodec.h"
    #include "libavutil/imgutils.h"

    #include "libavutil/frame.h"
    #include "libavutil/mem.h"

    #include "libswresample/swresample.h"
}

#define MAX_AUDIO_FRAME_SIZE  192000

ZenPlayerEngine::ZenPlayerEngine(QObject *parent)
    : QObject{parent}
{
    moveToThread(&_thread);
    connect(&_thread, &QThread::started, this, &ZenPlayerEngine::engine);
}

bool ZenPlayerEngine::open(const QString &fileName)
{
    _fileName = fileName;

    if (!init()) {
        return false;
    }

    _pause = true;
    _stopped = false;
    _thread.start();

    _timeline = 0;
    _pauseTimeLine = 0;

    return true;
}

void ZenPlayerEngine::play()
{
    qDebug() << Q_FUNC_INFO;
    _elapsedTimer.start();
    _pause = false;
    _pauseCondition.notify_one();
}

void ZenPlayerEngine::pause()
{
    _pauseTimeLine = _timeline;
    _pause = true;
    _pauseCondition.notify_one();
}

void ZenPlayerEngine::stop()
{
    _stopped = true;
    _pause = false;
    _pauseCondition.notify_one();
    _thread.quit();
    _thread.wait();
}

void ZenPlayerEngine::seek(uint64_t second)
{
    uint64_t frameNumber = av_rescale(second, _formatContext->streams[_videoIndex]->time_base.den, _formatContext->streams[_videoIndex]->time_base.num);
    frameNumber /= 1000;

    if(avformat_seek_file(_formatContext, _videoIndex, 0, frameNumber, frameNumber, AVSEEK_FLAG_FRAME) < 0) {
        processError("Could not file seek");
        return;
    }

    avcodec_flush_buffers(_videoCodecContext);
}

void ZenPlayerEngine::engine()
{
    qDebug() << Q_FUNC_INFO;
    int size = _videoCodecContext->width * _videoCodecContext->height;

    AVPacket packet;

    if(av_new_packet(&packet, size) != 0)
    {
        qWarning() << "New packet failed!";
    }

    _audioOutputDevice = _audioOutput->start();

    av_freep(&_frame->data[0]);
    av_frame_unref(_frame);

    while (!_stopped)
    {
        if(av_read_frame(_formatContext, &packet) >= 0)
        {
            if(packet.stream_index == _videoIndex)
            {
                int result = avcodec_send_packet(_videoCodecContext, &packet);
                if (result != 0)
                {
                    av_packet_unref(&packet);
                    continue;
                }

                result = avcodec_receive_frame(_videoCodecContext, _frame);
                if (result != 0)
                {
                    av_packet_unref(&packet);
                    continue;
                }

                sws_scale(_swsContext, static_cast<const uint8_t* const*>(_frame->data),
                          _frame->linesize, 0, _videoCodecContext->height, _frameRGB->data,
                          _frameRGB->linesize);

                QImage *image = new QImage((uchar*)_outBuffer, _videoCodecContext->width, _videoCodecContext->height, _frameRGB->linesize[0], QImage::Format_RGB888);
                Q_EMIT frameComplete(image);

                std::unique_lock<std::mutex> lock(_pauseMutex);
                _pauseCondition.wait(lock, [=]{return !_pause;});

                _timeline = _pauseTimeLine + _elapsedTimer.elapsed();
                int timestamp = av_rescale_q(packet.dts,_formatContext->streams[_videoIndex]->time_base, {1, 1000});
                int diff = timestamp - _timeline;
                if (diff > 0) {
                    QThread::msleep(diff);
                }
            }

            if (packet.stream_index == _audioIndex && !_pause) {

                int result = avcodec_send_packet (_audioCodecContext, & packet);
                if (result != 0)
                {
                    av_packet_unref (& packet);
                    continue;
                }

                result = avcodec_receive_frame (_audioCodecContext, _audioFramePCM);
                if (result != 0)
                {
                    av_packet_unref (& packet);
                    continue;
                }

                swr_convert(_audioConvertContext, &_audioBufferTemp, MAX_AUDIO_FRAME_SIZE, (const uint8_t **)_audioFramePCM->data, _audioFramePCM->nb_samples);

                _audioOutputDevice->write(reinterpret_cast<const char*>(_audioBufferTemp),_audioBufferSize);
            }
        }else {
           _stopped = true;
        }

        av_packet_unref(&packet);
    }
}

bool ZenPlayerEngine::init()
{
    _formatContext = avformat_alloc_context();

    if(avformat_open_input(&_formatContext, _fileName.toStdString().c_str(), NULL, NULL)) {
        processError("Can`t open file");
        return false;
    }

    if(avformat_find_stream_info(_formatContext, NULL) < 0)
    {
        processError("Could not find stream information");
        return false;
    }

    _videoIndex = -1;
    _audioIndex = -1;

    for(int i = 0; i < _formatContext->nb_streams; i++)
    {
        if(_formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            _videoIndex = i;
        }

        if(_formatContext->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_AUDIO)
        {
            _audioIndex = i;
        }
    }

    if(_videoIndex == -1)
    {
        processError("Could not find video stream");
        return false;
    }

    _videoCodecContext = _formatContext->streams[_videoIndex]->codec;
    _codec = avcodec_find_decoder(_videoCodecContext->codec_id);

    if(!_codec)
    {
        processError("could not find codec");
        return false;
    }

    _audioCodecContext = _formatContext->streams[_audioIndex]->codec;
    _audioCodec = avcodec_find_decoder(_audioCodecContext->codec_id);

    if (avcodec_open2(_videoCodecContext, _codec, NULL) < 0) {
        processError("Could not open codec.");
        return false;
    }

    if (avcodec_open2(_audioCodecContext, _audioCodec, NULL) < 0) {
        processError("Could not open video codec.");
        return false;
    }

    _frame     = av_frame_alloc();
    _frameRGB  = av_frame_alloc();

    quint32 size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, _videoCodecContext->width, _videoCodecContext->height, 1);

    _outBuffer = static_cast<uint8_t*>(av_malloc(size));
    av_image_fill_arrays(_frameRGB->data, _frameRGB->linesize, _outBuffer, AV_PIX_FMT_RGB24,  _videoCodecContext->width, _videoCodecContext->height, 1);

    _swsContext = sws_getContext(_videoCodecContext->width, _videoCodecContext->height, _videoCodecContext->pix_fmt,
                                 _videoCodecContext->width, _videoCodecContext->height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

    uint64_t outChannelLayout = AV_CH_LAYOUT_MONO;
    int outNbSamples = 1024;
    enum AVSampleFormat  sampleFMT = AV_SAMPLE_FMT_S16;
    int outSampleRate = 48000;
    int outChannels = av_get_channel_layout_nb_channels(outChannelLayout);

    _audioBufferSize = av_samples_get_buffer_size(nullptr, outChannels, outNbSamples, sampleFMT, 1);
    _audioBufferTemp = (uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);

    int64_t inChannelLayout = av_get_default_channel_layout(_audioCodecContext->channels);

    _audioFramePCM = av_frame_alloc ();

    _audioConvertContext = swr_alloc();
    _audioConvertContext = swr_alloc_set_opts(_audioConvertContext, outChannelLayout, sampleFMT, outSampleRate,
               inChannelLayout, _audioCodecContext->sample_fmt, _audioCodecContext->sample_rate, 0, nullptr);

    swr_init(_audioConvertContext);

    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::UnSignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());


    _audioOutput = new QAudioOutput(format);

    _audioFrame = av_frame_alloc();

    return true;
}

void ZenPlayerEngine::getFirstVideoFrame()
{
    AVPacket packet;

    int size = _videoCodecContext->width * _videoCodecContext->height;
    if(av_new_packet(&packet, size) != 0)
    {
        qWarning() << "New packet failed!";
    }

//    av_freep(&_frame->data[0]);
//    av_frame_unref(_frame);

    while (true)
    {
        if(av_read_frame(_formatContext, &packet) >= 0)
        {
            if(packet.stream_index == _videoIndex)
            {
                int result = avcodec_send_packet(_videoCodecContext, & packet);
                if (result != 0)
                {
                    av_packet_unref(&packet);
                    continue;
                }

                result = avcodec_receive_frame(_videoCodecContext, _frame);
                if (result != 0)
                {
                    av_packet_unref(&packet);
                    continue;
                }

                sws_scale(_swsContext, static_cast<const uint8_t* const*>(_frame->data),
                          _frame->linesize, 0, _videoCodecContext->height, _frameRGB->data,
                          _frameRGB->linesize);

                QImage *image = new QImage((uchar*)_outBuffer, _videoCodecContext->width, _videoCodecContext->height, _frameRGB->linesize[0], QImage::Format_RGB888);
                image->save("C:/log/firstframe.png");
                Q_EMIT frameComplete(image);
                break;
            }
        }

        av_packet_unref(&packet);
    }

    qDebug() << Q_FUNC_INFO << "complete";
}

void ZenPlayerEngine::processError(const QString &errorString)
{
    Q_EMIT error(errorString);
}
