#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define VERSION_SERIAL 1.2f

#include <QMainWindow>
#include <QSerialPort>
#include <QMessageBox>
#include <QLabel>
#include <QtDebug>
#include <QWidget>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFile>
#include <QFileDialog>
#include <QMutex>

#include "serialport.h"
#include "settingsdialog.h"


QT_BEGIN_NAMESPACE

namespace Ui { class MainWindow; }

QT_END_NAMESPACE

class SerialPort;

class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    SerialPort::Settings getSerialInfo();
    QString getSerialError();

    // All struct

    typedef struct GpsData{

        long altitulde_mm = 0;
        long latitude_mdeg = 0;
        long longitude_mdeg = 0;

        uint8_t hourGPS = 0;
        uint8_t minuteGPS = 0;
        uint8_t secondGPS = 0;

        uint8_t gpsRun = 0;

    }GpsData;

    typedef struct Bmp280{

        float temp = 0;
        float pres = 0;

        uint8_t bmp280Run = 0;

    }Bmp280;

    typedef struct Ds18b20{

        float tempMotor = 0;
        uint8_t ds18B20Run = 0;

    }Ds18b20;

    typedef struct Ds3231S{

        uint8_t secondRTC;
        uint8_t minuteRTC;
        uint8_t hourRTC;

        uint8_t ds3231sRun = 0;

    }Ds3231;

    typedef struct Mpu6050{

        float gyroX = 0;
        float gyroY = 0;
        float gyroZ = 0;
        float AccX = 0;
        float AccY = 0;
        float AccZ = 0;

        uint8_t mpu6050Run = 0;

    }Mpu6050;

    typedef struct AllData{

        GpsData gpsVar;
        Bmp280 bmp280Var;
        Ds18b20 ds18b20Var;
        Ds3231 ds3231Var;
        Mpu6050 mpu6050Var;

        uint8_t seqMes = 0;

    }AllData;

public slots :

    void handleErrorShow(QString error);
    void settingShow();
    void setSerialSettings();

    void opennedSerial(SerialPort::Settings p);
    void closedSerial();
    void openSerialPort();
    void closeSerialPort();

    void receptionData(bool receptionChek, const QByteArray data);

    void about();

signals:

    void setSerialSettingsSig(SerialPort::Settings);
    void serialOppened(SerialPort::Settings p);
    void serialClosed();

private:
    void initActionsConnections();
    void initActionsConnectionsPrio();

    void activateButtonSerial();
    void disactivateButtonSerial();

    void showStatusMessage(const QString &stringConnection);

    Ui::MainWindow *ui;
    QObject *m_parent;

    QLabel *m_status = nullptr;
    QString *m_connection;
    QString *m_versionSW;
    SerialPort *m_serial = nullptr;
    bool m_serialRun;

    SettingsDialog *m_settings = nullptr;
    SerialPort::Settings *m_settingsInfo = nullptr;

    AllData m_data;


};
#endif // MAINWINDOW_H
