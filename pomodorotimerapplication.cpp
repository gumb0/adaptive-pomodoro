#include "pomodorotimerapplication.h"

#include <QPainter>
#include <QSound>

namespace
{
    const QString IdleIconPath{":/resources/Tomato-48.ico"};
    const QString NotificationSoundPath{":/resources/kitchen_timer_bell_ring.wav"};

    const QString ActionStartPomodoro{QObject::tr("Start %1 min pomodoro")};
    const QString ActionExit{QObject::tr("Exit")};

    const QString PomodoroEndMessage{QObject::tr("Pomodoro is over")};
    const QString BreakEndMessage{QObject::tr("Break is over")};
    const QString IdleAlertMessage{QObject::tr("You should get back to work!")};

    const int IconUpdateIntervalMsec{60 * 1000};
    const int WorkIntervalDefaltMinutes{25};
    const int WorkIntervalStepMinutes{5};
    const int RestIntervalMinutes{5};
    const int IdleAlertIntervalMsec{5 * 60 * 1000};

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
    mWorkIntervalMinutes{WorkIntervalDefaltMinutes},
    mMinutesLeft{0},
    mCurrentState{PomodoroState::Idle}
{
    setupMenu();

    mSystemTrayIcon.show();

    mUpdateTimer.setInterval(IconUpdateIntervalMsec);
    connect(&mUpdateTimer, &QTimer::timeout, this, &PomodoroTimerApplication::onUpdateTimer);

    mIdleTimer.setInterval(IdleAlertIntervalMsec);
    connect(&mIdleTimer, &QTimer::timeout, this, &PomodoroTimerApplication::onIdleTimer);
}

QFont PomodoroTimerApplication::createFont()
{
    QFont font{FontFamily};
    font.setPixelSize(TextIconSize - TextMargin * 2);

    return font;
}

void PomodoroTimerApplication::setupMenu()
{
    mMenu.clear();

    addStartPomodoroItem(mWorkIntervalMinutes - WorkIntervalStepMinutes);
    addStartPomodoroItem(mWorkIntervalMinutes);
    addStartPomodoroItem(mWorkIntervalMinutes + WorkIntervalStepMinutes);

    QAction* action{new QAction(ActionExit, this)};
    connect(action, &QAction::triggered, this, &PomodoroTimerApplication::onExit);

    mMenu.addAction(action);

    mSystemTrayIcon.setContextMenu(&mMenu);
}

void PomodoroTimerApplication::addStartPomodoroItem(int workInterval)
{
    QAction* action{new QAction(ActionStartPomodoro.arg(workInterval), this)};
    connect(action, &QAction::triggered, [this, workInterval]() { onStartPomodoro(workInterval); });

    if (workInterval <= 0 || workInterval >= 99)
        action->setEnabled(false);

    mMenu.addAction(action);
}

void PomodoroTimerApplication::onStartPomodoro(int workInterval)
{
    mWorkIntervalMinutes = workInterval;
    setupMenu();
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

void PomodoroTimerApplication::onUpdateTimer()
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

void PomodoroTimerApplication::onIdleTimer()
{
    QSound::play(NotificationSoundPath);
    mSystemTrayIcon.showMessage(QString(), IdleAlertMessage, QSystemTrayIcon::NoIcon);
    mIdleTimer.start();
}

void PomodoroTimerApplication::startWork()
{
    mIdleTimer.stop();
    mCurrentState = PomodoroState::Work;
    mMinutesLeft = mWorkIntervalMinutes;
    mUpdateTimer.start();
}

void PomodoroTimerApplication::startRest()
{
    mCurrentState = PomodoroState::Rest;
    mMinutesLeft = RestIntervalMinutes;
    mUpdateTimer.start();
}

void PomodoroTimerApplication::startIdle()
{
    mCurrentState = PomodoroState::Idle;
    mMinutesLeft = 0;
    mUpdateTimer.stop();
    mIdleTimer.start();
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
