#include "pomodorotimerapplication.h"

#include <QMessageBox>

namespace
{
    const QString ErrorTitle{QObject::tr("Error")};
    const QString ErrorPlatrofmNotSupported{QObject::tr("Sorry, your operating system is not supported.")};
}

int main(int argc, char *argv[])
{
    PomodoroTimerApplication a(argc, argv);

    if (!PomodoroTimerApplication::isPlatformSupported())
    {
        QMessageBox::critical(nullptr, ErrorTitle, ErrorPlatrofmNotSupported);
        return -1;
    }

    return a.exec();
}
