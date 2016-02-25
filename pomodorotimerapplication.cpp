#include "pomodorotimerapplication.h"

#include <QPainter>
#include <QTimer>

PomodoroTimerApplication::PomodoroTimerApplication(int& argc, char** argv) : QApplication{argc, argv},
    mIdleIcon{":/resources/tomato_icon.png"},
    mSystemTrayIcon{mIdleIcon, this},
    mMenu{nullptr},
    mTimer{new QTimer(this)},
    mMinutesLeft{0},
    mCurrentState{PomodoroState::Idle}
{
    // TODO check whether a system tray is present on the user's desktop

    QAction* action1{new QAction("Start pomodoro", this)};
    mMenu.addAction(action1);

    connect(action1, &QAction::triggered, this, &PomodoroTimerApplication::onStartPomodoro);

    QAction* action{new QAction("Exit", this)};
    connect(action, &QAction::triggered, this, &PomodoroTimerApplication::onExit);

    mMenu.addAction(action);

    mSystemTrayIcon.setContextMenu(&mMenu);

    mSystemTrayIcon.show();

    connect(mTimer, &QTimer::timeout, this, &PomodoroTimerApplication::onTimer);
}

void PomodoroTimerApplication::onStartPomodoro()
{
    mTimer->start(60 * 1000);
    mMinutesLeft = 25;
    mCurrentState = PomodoroState::Work;
    updateSystemTrayIcon();
}

void PomodoroTimerApplication::updateSystemTrayIcon()
{
    QRect geometry = mSystemTrayIcon.geometry();

    QPixmap pixmap{geometry.size()};
    pixmap.fill(mCurrentState == PomodoroState::Work ? Qt::darkRed : Qt::darkGreen);

    QPainter painter{&pixmap};

    painter.setFont(QFont("Helvetica", 18));
    painter.drawText(2, 0, geometry.width() - 4, geometry.height(), Qt::AlignCenter, QString::number(mMinutesLeft));

    QIcon icon2{pixmap};

    mSystemTrayIcon.setIcon(icon2);
}

void PomodoroTimerApplication::onTimer()
{
    --mMinutesLeft;

    if (mMinutesLeft == 0)
    {
        if (mCurrentState == PomodoroState::Work)
        {
            mMinutesLeft = 5;
            mCurrentState = PomodoroState::Break;
            QApplication::beep();
            mTimer->start();
        }
        else if (mCurrentState == PomodoroState::Break)
        {
            mCurrentState = PomodoroState::Break;
            QApplication::beep();
            mTimer->stop();
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
