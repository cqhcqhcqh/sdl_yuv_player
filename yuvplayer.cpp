#include "yuvplayer.h"
#include <QDebug>

YuvPlayer::YuvPlayer(QWidget *parent)
    : QWidget{parent}
{
    setup();
}

void YuvPlayer::setup() {
    SDL_Renderer *ctx = nullptr;
    SDL_Window *window = nullptr;

    int ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret < 0) {
        qDebug() << "SDL_Init error" << SDL_GetError();
        return;
    }
    window = SDL_CreateWindowFrom((void *)winId());
    if (window == nullptr) {
        qDebug() << "SDL_CreateWindow error" << SDL_GetError();
        return;
    }

    ctx = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ctx == nullptr) {
        ctx = SDL_CreateRenderer(window, -1, 0);
    }

    if (ctx == nullptr) {
        SDL_DestroyWindow(_window);
        qDebug() << "SDL_CreateRenderer error" << SDL_GetError();
        return;
    }

    _window = window;
    _ctx = ctx;
}

static const std::map<AVPixelFormat, SDL_PixelFormatEnum> fmtMap = {
    {AV_PIX_FMT_YUV420P, SDL_PIXELFORMAT_IYUV},
    {AV_PIX_FMT_YUYV422, SDL_PIXELFORMAT_YUY2},
    {AV_PIX_FMT_NONE, SDL_PIXELFORMAT_UNKNOWN},
};

void YuvPlayer::replace(YuvFile &in) {
    SDL_Texture *tex = SDL_CreateTexture(_ctx,
                                         fmtMap.find(in.fmt)->second,
                                         SDL_TEXTUREACCESS_STREAMING,
                                         in.width,
                                         in.height);
    if (tex == nullptr) {
        qDebug() << "SDL_CreateTexture error" << SDL_GetError();
        return;
    }

    // 如果 YuvFile 的 path 是 char *const 来修饰的话, _in = in; 赋值语句就无法成功
    _in = in;
    _tex = tex;
}

void YuvPlayer::play() {
    _startPlayTimerId = startTimer(1000 / 30);
    // 打开文件
    _file.setFileName(_in.path);
    int ret = _file.open(QFile::ReadOnly);
    if (ret == 0) {
        qDebug() << "QFile open error" << _in.path;
        return;
    }
}

void YuvPlayer::timerEvent(QTimerEvent *event) {
    int image_buffer_size = av_image_get_buffer_size(_in.fmt, _in.width, _in.height, 1);
    qDebug() << "image_buffer_size" << image_buffer_size;
    char image_buffer[image_buffer_size];
    int bytesPerRow = _in.width;
    if (_file.read(image_buffer, image_buffer_size) > 0) {
        SDL_UpdateTexture(_tex, NULL, image_buffer, bytesPerRow);
        SDL_RenderCopy(_ctx, _tex, nullptr, nullptr);
        SDL_RenderPresent(_ctx);
    } else {
        pause();
    }
}

void YuvPlayer::pause() {
    if (_startPlayTimerId == 0) return;
    killTimer(_startPlayTimerId);
}

void YuvPlayer::destory() {
    pause();
    _file.close();
    SDL_DestroyTexture(_tex);
    SDL_DestroyWindow(_window);
    SDL_DestroyRenderer(_ctx);
}

/// todo
void YuvPlayer::seek() {

}
