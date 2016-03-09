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
    void onStartPomodoro();
    void onTimer();
    void onExit();

private:
    QFont createFont();
    void setupMenu();
    void updateSystemTrayIcon();
    QIcon getCurrentIcon() const;
    QIcon createNumberIcon(PomodoroState state, int minutes) const;

    void startWork();
    void startRest();
    void startIdle();

private:
    const QIcon mIdleIcon;
    QSystemTrayIcon mSystemTrayIcon;
    QFont mFont;
    QMenu mMenu;
    QTimer mTimer;
    int mMinutesLeft;
    PomodoroState mCurrentState;
};

#endif // POMODOROTIMERAPPLICATION_H
