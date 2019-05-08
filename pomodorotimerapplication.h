#ifndef POMODOROTIMERAPPLICATION_H
#define POMODOROTIMERAPPLICATION_H

#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTimer>

enum class PomodoroState
{
    Idle,
    Work,
    Rest
};

class PomodoroTimerApplication : public QApplication
{
    Q_OBJECT

public:
    static bool isPlatformSupported();

public:
    PomodoroTimerApplication(int& argc, char** argv);

public slots:
    void onStartPomodoro(int workInterval);
    void onUpdateTimer();
    void onIdleTimer();
    void onExit();

private:
    QFont createFont();
    void setupMenu();
    void addStartPomodoroItem(int workInterval);
    void updateSystemTrayIcon();
    QIcon getCurrentIcon() const;
    QIcon createNumberIcon(PomodoroState state, int minutes) const;

    void startWork();
    void startRest();
    void startIdle();

    void showEndMessage(PomodoroState state);

private:
    const QIcon mIdleIcon;
    QSystemTrayIcon mSystemTrayIcon;
    QFont mFont;
    QMenu mMenu;
    QTimer mUpdateTimer;
    QTimer mIdleTimer;
    int mWorkIntervalMinutes;
    int mMinutesLeft;
    PomodoroState mCurrentState;
};

#endif // POMODOROTIMERAPPLICATION_H
