#ifndef YUVPLAYER_H
#define YUVPLAYER_H

#include <QWidget>
#include <QFile>
#include <SDL2/SDL.h>
extern "C" {
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
}

typedef struct {
    char *path;
    int width;
    int height;
    AVPixelFormat fmt;
    SDL_PixelFormat pfmt;
} YuvFile;

class YuvPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit YuvPlayer(QWidget *parent = nullptr);
    void play();
    void pause();
    void destory();
    void seek();
    void replace(YuvFile &in);
signals:
private:
    void setup();
    void timerEvent(QTimerEvent *event);
    SDL_Renderer *_ctx = nullptr;
    SDL_Window *_window = nullptr;
    SDL_Texture *_tex = nullptr;
    YuvFile _in;
    QFile _file;
    int _startPlayTimerId = 0;
};

#endif // YUVPLAYER_H
