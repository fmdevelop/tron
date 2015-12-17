/**
 * Modified COPY OF:
 * https://github.com/mifit/mifit/blob/master/libs/opengl/Text.h
 */

#ifndef TEXT_H
#define TEXT_H

#include <QFont>
#include <QString>
#include <QFontMetrics>

namespace tron {
namespace rendering {

class TextPrivate;

class Text
{
public:

    Text(const QFont &f);
    virtual ~Text();

    QFont font() const;
    QFontMetrics fontMetrics() const;

    void renderText(float x, float y, const QString &text, bool alignCenter = true);


private:
    Q_DISABLE_COPY(Text)

    TextPrivate *const d;

};


}
}


#endif // TEXT_H
