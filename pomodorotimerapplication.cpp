#include "pomodorotimerapplication.h"

#include <QPainter>

namespace
{
    // TODO choose better icon
    const QString IdleIconPath{":/resources/tomato_icon.png"};

    const QString ActionStartPomodoro{QObject::tr("Start pomodoro")};
    const QString ActionExit{QObject::tr("Exit")};

    const int IconUpdateIntervalMsec{60 * 1000};
    const int WorkIntervalMinutes{25};
    const int RestIntervalMinutes{5};

    const QColor ColorWork{Qt::darkRed};
    const QColor ColorRest{Qt::darkGreen};

    // TODO choose better font
    const QFont IconFont{"Helvetica", 18};
}

PomodoroTimerApplication::PomodoroTimerApplication(int& argc, char** argv) : QApplication{argc, argv},
    mIdleIcon{IdleIconPath},
    mSystemTrayIcon{mIdleIcon},
    mMinutesLeft{0},
    mCurrentState{PomodoroState::Idle}
{
    // TODO check whether a system tray is present on the user's desktop

    setupMenu();

    mSystemTrayIcon.show();

    connect(&mTimer, &QTimer::timeout, this, &PomodoroTimerApplication::onTimer);
}

void PomodoroTimerApplication::setupMenu()
{
    QAction* action1{new QAction(ActionStartPomodoro, this)};
    mMenu.addAction(action1);

    connect(action1, &QAction::triggered, this, &PomodoroTimerApplication::onStartPomodoro);

    QAction* action{new QAction(ActionExit, this)};
    connect(action, &QAction::triggered, this, &PomodoroTimerApplication::onExit);

    mMenu.addAction(action);

    mSystemTrayIcon.setContextMenu(&mMenu);
}

void PomodoroTimerApplication::onStartPomodoro()
{
    mTimer.start(IconUpdateIntervalMsec);
    mMinutesLeft = WorkIntervalMinutes;
    mCurrentState = PomodoroState::Work;
    updateSystemTrayIcon();
}

void PomodoroTimerApplication::updateSystemTrayIcon()
{
    if (mCurrentState == PomodoroState::Idle)
    {
        mSystemTrayIcon.setIcon(mIdleIcon);
        return;
    }

    QRect geometry = mSystemTrayIcon.geometry();

    QPixmap pixmap{geometry.size()};
    pixmap.fill(mCurrentState == PomodoroState::Work ? ColorWork : ColorRest);

    QPainter painter{&pixmap};

    painter.setFont(IconFont);
    // TODO choose font color
    painter.drawText(2, 0, geometry.width() - 4, geometry.height(), Qt::AlignCenter,
        QString::number(mMinutesLeft));

    QIcon icon{pixmap};

    mSystemTrayIcon.setIcon(icon);
}

void PomodoroTimerApplication::onTimer()
{
    --mMinutesLeft;

    if (mMinutesLeft == 0)
    {
        if (mCurrentState == PomodoroState::Work)
        {
            mMinutesLeft = RestIntervalMinutes;
            mCurrentState = PomodoroState::Rest;
            QApplication::beep();
            mTimer.start();
        }
        else if (mCurrentState == PomodoroState::Rest)
        {
            mCurrentState = PomodoroState::Idle;
            QApplication::beep();
            mTimer.stop();
        }
        else
        {
            Q_ASSERT(false);
        }

    }

    updateSystemTrayIcon();
}

void PomodoroTimerApplication::onExit()
{
    exit();
}
