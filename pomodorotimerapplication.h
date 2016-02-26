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
    PomodoroTimerApplication(int& argc, char** argv);

public slots:
    void onStartPomodoro();
    void onTimer();
    void onExit();

private:
    void setupMenu();
    void updateSystemTrayIcon();

private:
    const QIcon mIdleIcon;
    QSystemTrayIcon mSystemTrayIcon;
    QMenu mMenu;
    QTimer mTimer;
    int mMinutesLeft;
    PomodoroState mCurrentState;
};

#endif // POMODOROTIMERAPPLICATION_H
