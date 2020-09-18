/*
 * Copyright (C) 2019-2020 Jolla Ltd.
 * Copyright (C) 2019-2020 Slava Monich <slava@monich.com>
 *
 * You may use this file under the terms of BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
 *      distribution.
 *   3. Neither the names of the copyright holders nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>

#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include <QGuiApplication>
#include <QDebug>

#ifdef HAVE_SILICA
#  include <silicatheme.h>
#else
#  include <dlfcn.h>
#  ifdef __aarch64__
#    define LIBDIR "/usr/lib64"
#  else
#    define LIBDIR "/usr/lib"
#  endif

#  define SILICA_SO LIBDIR "/libsailfishsilica.so.1"
typedef QObject* (*SilicaInstanceFunc)(void);
typedef qreal (*SilicaIconSizeMediumFunc)(QObject*);
#endif // HAVE_SILICA

// Converts SVGs to PNGs of the size Theme.iconSizeMedium
int main(int argc, char* argv[])
{
    setenv("QT_QPA_PLATFORM", "minimal", 1);
    QGuiApplication app(argc, argv);

#ifdef HAVE_SILICA
    const int size = Silica::Theme::instance()->iconSizeMedium();
#else
    // It gets slightly more complicated if we can't link againt silica :(
    int size = 0;
    static void* handle = dlopen(SILICA_SO, RTLD_LAZY);
    if (handle) {
        SilicaInstanceFunc instanceFunc = (SilicaInstanceFunc)
            dlsym(handle, "_ZN6Silica5Theme8instanceEv");
        SilicaIconSizeMediumFunc iconSizeMediumFunc = (SilicaIconSizeMediumFunc)
            dlsym(handle, "_ZNK6Silica5Theme14iconSizeMediumEv");
        if (instanceFunc) {
            QObject* instance = instanceFunc();
            if (instance) {
                size = iconSizeMediumFunc(instance);
            }
        }
    }
#endif

    if (size) {
        const QImage::Format format = QImage::Format_ARGB32_Premultiplied;
        for (int i = 1; i < argc; i++) {
            QString file(argv[i]);
            QSvgRenderer renderer(file);

            if (renderer.isValid()) {
                QImage out(QSize(size, size), format);
                out.fill(0);

                QPainter painter(&out);
                painter.setRenderHint(QPainter::Antialiasing);
                painter.setRenderHint(QPainter::TextAntialiasing);
                painter.setRenderHint(QPainter::SmoothPixmapTransform);
                renderer.render(&painter, out.rect());

                file.replace(".svg", ".png");
                if (!out.save(file)) {
                    qWarning() << "Failed to save" << qPrintable(file);
                }
            } else {
                qWarning() << "Not a valid SVG file:" << qPrintable(file);
            }
        }
        return 0;
    } else {
        return 1;
    }
}
