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
    void onStartRest();
    void onTimer();
    void onExit();

private:
    QFont createFont();
    void setupWorkMenu();
    void setupRestMenu();
    void addStartPomodoroItem();
    void addRestartPomodoroItem();
    void addStartRestItem();
    void addExitItem();
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
    QTimer mTimer;
    int mRestIntervalMinutes;
    int mMinutes;
    PomodoroState mCurrentState;
};

#endif // POMODOROTIMERAPPLICATION_H
