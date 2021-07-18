#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QSemaphore>
#include <QStack>
#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QThread>
#include <QDateTime>

class SettingsDialog;

class SerialPort : public QThread
{
    Q_OBJECT

public:
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
    };

    SerialPort();
    ~SerialPort();

    bool openSerial();
    void handleError(QSerialPort::SerialPortError error);

    void closeSerial();

    bool checkOpenSerial() const;
    Settings settingsInfo() const;
    QString serialError() const;
    void setSerialRun(bool onoff);
    void run() override;

public slots:
    void settingUpdate(SerialPort::Settings settingPort);
    void readingData();

signals:
    void errorEmit(QString);
    void dataEmit(bool receptionChek, QByteArray data = NULL);
    void serialOpenned(SerialPort::Settings p);
    void serialClosed();

private:
    Settings *m_settingsPort = nullptr;
    QSerialPort *m_serial = nullptr;
    QTimer *m_waitTimerReponse = nullptr;

    int m_waitTimeout = 1500;
    bool m_serialRun;

    QMutex mut;
};

#endif // SERIALPORT_H
