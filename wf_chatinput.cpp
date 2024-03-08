/**********************************
 *
 *       WeFish Chat Input
 *
 * Author: Kim.He@tymphany.com
 * date  : 11/24 2022
 *
***********************************/
#include "wf_chatinput.h"

#include <QMimeData>
#include <QFileInfo>
#include <QImageReader>
#include <QKeyEvent>

WF_ChatInput::WF_ChatInput(QWidget *parent) : QTextEdit(parent)
{
    this->setStyleSheet("background: rgb(245,245,245); border:0px");
    QFont font = QFont("Microsoft Yahei");
    font.setPixelSize(15);
    this->setFont(font);
}

WF_ChatInput::~WF_ChatInput()
{
}

bool WF_ChatInput::canInsertFromMimeData(const QMimeData *source) const
{
    return source->hasImage() || source->hasUrls() || QTextEdit::canInsertFromMimeData(source);
}

void WF_ChatInput::insertFromMimeData(const QMimeData *source)
{
    if (source->hasImage()) {
        static int i = 1;
        QUrl url(QString("dropped_image_%1").arg(i++));
        dropImage(url, qvariant_cast<QImage>(source->imageData()));
    } else if (source->hasUrls()) {
        foreach (QUrl url, source->urls()) {
            QFileInfo info(url.toLocalFile());
            if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1()))
                dropImage(url, QImage(info.filePath()));
            else
                dropTextFile(url);
        }
    } else {
        QTextEdit::insertFromMimeData(source);
    }
}

void WF_ChatInput::dropImage(const QUrl &url, const QImage &image)
{
    if (!image.isNull()) {
        document()->addResource(QTextDocument::ImageResource, url, image);
        textCursor().insertImage(url.toString());
    }
}

void WF_ChatInput::dropTextFile(const QUrl &url)
{
    QFile file(url.toLocalFile());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        textCursor().insertText(file.readAll());
}
