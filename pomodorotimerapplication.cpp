#include "pomodorotimerapplication.h"

#include <QPainter>
#include <QSound>

namespace
{
    const QString IdleIconPath{":/resources/Tomato-48.ico"};
    const QString NotificationSoundPath{":/resources/whoosh.wav"};

    const QString ActionStartPomodoro{QObject::tr("Start pomodoro")};
    const QString ActionStartRest{QObject::tr("Break for %1 min")};
    const QString ActionExit{QObject::tr("Exit")};

    const QString PomodoroEndMessage{QObject::tr("Pomodoro is over")};
    const QString BreakEndMessage{QObject::tr("Break is over")};

    const int IconUpdateIntervalMsec{60 * 1000};
    const int WorkIntervalDefaltMinutes{25};

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
    mRestIntervalMinutes{0},
    mMinutes{0},
    mCurrentState{PomodoroState::Idle}
{
    setupRestMenu();

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

void PomodoroTimerApplication::setupRestMenu()
{
    mMenu.clear();

    addStartPomodoroItem();
    addExitItem();

    mSystemTrayIcon.setContextMenu(&mMenu);
}

void PomodoroTimerApplication::addStartPomodoroItem()
{
    QAction* action{new QAction(ActionStartPomodoro, this)};
    connect(action, &QAction::triggered, [this]() { onStartPomodoro(); });

    mMenu.addAction(action);
}

void PomodoroTimerApplication::addExitItem()
{
    QAction* action{new QAction(ActionExit, this)};
    connect(action, &QAction::triggered, this, &PomodoroTimerApplication::onExit);

    mMenu.addAction(action);
}

void PomodoroTimerApplication::onStartPomodoro()
{
    setupWorkMenu();
    startWork();
    updateSystemTrayIcon();
}

void PomodoroTimerApplication::setupWorkMenu()
{
    mMenu.clear();

    addStartRestItem();
    addExitItem();

    mSystemTrayIcon.setContextMenu(&mMenu);
}

void PomodoroTimerApplication::addStartRestItem()
{
    QAction* action{new QAction(ActionStartRest.arg(mRestIntervalMinutes), this)};
    connect(action, &QAction::triggered, [this]() { onStartRest(); });

    if (mRestIntervalMinutes == 0)
        action->setEnabled(false);

    mMenu.addAction(action);
}

void PomodoroTimerApplication::onStartRest()
{
    setupRestMenu();
    startRest();
    updateSystemTrayIcon();
}

void PomodoroTimerApplication::updateSystemTrayIcon()
{
    mSystemTrayIcon.setIcon(getCurrentIcon());
}

QIcon PomodoroTimerApplication::getCurrentIcon() const
{
    return mCurrentState == PomodoroState::Idle ? mIdleIcon :
        createNumberIcon(mCurrentState, mMinutes);
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
    // TODO update menu with number of minutes of potential break
    // TODO disable Break menu if mMinutes < 3

    if (mCurrentState == PomodoroState::Work)
    {
        ++mMinutes;
        if (mMinutes / 3 != mRestIntervalMinutes)
        {
            mRestIntervalMinutes = mMinutes / 3;
            setupWorkMenu();
        }
    }
    else if (mCurrentState == PomodoroState::Rest)
    {
        --mMinutes;

        if (mMinutes == 0)
        {
            QSound::play(NotificationSoundPath);
            showEndMessage(mCurrentState);

            startIdle();
        }
    }
    else
        Q_ASSERT(false);

    updateSystemTrayIcon();
}

void PomodoroTimerApplication::startWork()
{
    mCurrentState = PomodoroState::Work;
    mMinutes = 0;
    mRestIntervalMinutes = 0;
    mTimer.start();
}

void PomodoroTimerApplication::startRest()
{
    mCurrentState = PomodoroState::Rest;
    mMinutes = mRestIntervalMinutes;
    mTimer.start();
}

void PomodoroTimerApplication::startIdle()
{
    mCurrentState = PomodoroState::Idle;
    mMinutes = 0;
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
