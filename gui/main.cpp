#include <glider_gui.h>
#include <QPalette>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    glider_gui gg;
    gg.resize(700,400);
    QPalette Pal;
    // устанавливаем цвет фона
    Pal.setColor(QPalette::Background, Qt::lightGray);
    gg.setAutoFillBackground(true);
    gg.setPalette(Pal);
    gg.show();
    return a.exec();
}
