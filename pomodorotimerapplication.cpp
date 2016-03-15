#include "pomodorotimerapplication.h"

#include <QPainter>
#include <QSound>

namespace
{
    const QString IdleIconPath{":/resources/Tomato-48.ico"};
    const QString NotificationSoundPath{":/resources/kitchen_timer_bell_ring.wav"};

    const QString ActionStartPomodoro{QObject::tr("Start pomodoro")};
    const QString ActionExit{QObject::tr("Exit")};

    const QString PomodoroEndMessage{QObject::tr("Pomodoro is over")};
    const QString BreakEndMessage{QObject::tr("Break is over")};

    const int IconUpdateIntervalMsec{60 * 1000};
    const int WorkIntervalMinutes{25};
    const int RestIntervalMinutes{5};

    const QColor ColorWork{Qt::darkRed};
    const QColor ColorRest{Qt::darkGreen};

    const int TextIconSize{128};
    const int TextMargin{10};
    const QString FontFamily{"arial"};
    const QColor ColorText{Qt::white};
}

bool PomodoroTimerApplication::isPlatformSupported()
{
    return QSystemTrayIcon::isSystemTrayAvailable();
}

PomodoroTimerApplication::PomodoroTimerApplication(int& argc, char** argv) : QApplication{argc, argv},
    mIdleIcon{IdleIconPath},
    mSystemTrayIcon{mIdleIcon},
    mFont{createFont()},
    mMinutesLeft{0},
    mCurrentState{PomodoroState::Idle}
{
    setupMenu();

    mSystemTrayIcon.show();

    mTimer.setInterval(IconUpdateIntervalMsec);
    connect(&mTimer, &QTimer::timeout, this, &PomodoroTimerApplication::onTimer);
}

QFont PomodoroTimerApplication::createFont()
{
    QFont font{FontFamily};
    font.setPixelSize(TextIconSize - TextMargin * 2);

    return font;
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
    startWork();
    updateSystemTrayIcon();
}

void PomodoroTimerApplication::updateSystemTrayIcon()
{
    mSystemTrayIcon.setIcon(getCurrentIcon());
}

QIcon PomodoroTimerApplication::getCurrentIcon() const
{
    return mCurrentState == PomodoroState::Idle ? mIdleIcon :
        createNumberIcon(mCurrentState, mMinutesLeft);
}

QIcon PomodoroTimerApplication::createNumberIcon(PomodoroState state, int minutes) const
{
    Q_ASSERT(state == PomodoroState::Work || state == PomodoroState::Rest);

    QPixmap pixmap{TextIconSize, TextIconSize};
    pixmap.fill(state == PomodoroState::Work ? ColorWork : ColorRest);

    QPainter painter{&pixmap};

    painter.setFont(mFont);
    painter.setPen(ColorText);
    const int sizeWithoutMargin = TextIconSize - 2 * TextMargin;
    painter.drawText(TextMargin, TextMargin, sizeWithoutMargin, sizeWithoutMargin, Qt::AlignCenter,
        QString::number(minutes));

    return QIcon{pixmap};
}

void PomodoroTimerApplication::onTimer()
{
    --mMinutesLeft;

    if (mMinutesLeft == 0)
    {
        QSound::play(NotificationSoundPath);
        showEndMessage(mCurrentState);

        switch (mCurrentState)
        {
        case (PomodoroState::Work):
            startRest();
            break;
        case (PomodoroState::Rest):
            startIdle();
            break;
        default:
            Q_ASSERT(false);
        }
    }

    updateSystemTrayIcon();
}

void PomodoroTimerApplication::startWork()
{
    mCurrentState = PomodoroState::Work;
    mMinutesLeft = WorkIntervalMinutes;
    mTimer.start();
}

void PomodoroTimerApplication::startRest()
{
    mCurrentState = PomodoroState::Rest;
    mMinutesLeft = RestIntervalMinutes;
    mTimer.start();
}

void PomodoroTimerApplication::startIdle()
{
    mCurrentState = PomodoroState::Idle;
    mMinutesLeft = 0;
    mTimer.stop();
}

void PomodoroTimerApplication::showEndMessage(PomodoroState state)
{
    Q_ASSERT(state == PomodoroState::Work || state == PomodoroState::Rest);

    const QString message = state == PomodoroState::Work ? PomodoroEndMessage : BreakEndMessage;
    mSystemTrayIcon.showMessage(QString(), message, QSystemTrayIcon::NoIcon);
}

void PomodoroTimerApplication::onExit()
{
    exit();
}
