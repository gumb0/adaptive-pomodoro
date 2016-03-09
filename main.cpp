#include "pomodorotimerapplication.h"

#include <QMessageBox>

namespace
{
    const QString ErrorTitle{QObject::tr("Error")};
    const QString ErrorPlatrofmNotSupported{QObject::tr("Sorry, your operating system is not supported.")};
}

int main(int argc, char *argv[])
{
    if (!PomodoroTimerApplication::isPlatformSupported())
    {
        QMessageBox::critical(nullptr, ErrorTitle, ErrorPlatrofmNotSupported);
        return -1;
    }


    PomodoroTimerApplication a(argc, argv);

    return a.exec();
}
