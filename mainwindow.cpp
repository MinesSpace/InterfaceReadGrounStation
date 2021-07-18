#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow), m_status(new QLabel), m_settingsInfo(new SerialPort::Settings)
{
    ui->setupUi(this);

    qDebug() << "Window :" << QThread::currentThread() << Qt::endl;

    m_serial = new SerialPort;
    m_serialRun = false;
    m_serial->start();

    initActionsConnectionsPrio();

    m_settings = new SettingsDialog;
    setSerialSettings();

    m_connection = new QString;

    m_status = new QLabel;
    ui->statusbar->addWidget(m_status);

    initActionsConnections();
    disactivateButtonSerial();

    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss") << "][MAINWINDOW] " << QThread::currentThread();
}

MainWindow::~MainWindow()
{
    /*m_serial->quit();
    m_worker->quit();
    m_serial->terminate();
    m_worker->terminate();*/

    delete m_settings;
    delete m_settingsInfo;
    delete m_status;
    delete ui;
}

void MainWindow::initActionsConnectionsPrio(){

    connect(this, SIGNAL(setSerialSettingsSig(SerialPort::Settings)), m_serial, SLOT(settingUpdate(SerialPort::Settings)));
    connect(m_serial, SIGNAL(errorEmit(QString)), this, SLOT(handleErrorShow(QString)));

}

void MainWindow::initActionsConnections()
{
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::showSetting); // set setting serial
    connect(m_settings, &SettingsDialog::applyParameter, this, &MainWindow::setSerialSettings);

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);

    connect(m_serial, &SerialPort::serialOpenned, this, &MainWindow::opennedSerial);
    //connect(m_serial, SIGNAL(serialOpenned(SerialPort::Settings)), this, SLOT(opennedSerial(SerialPort::Settings)));
    connect(m_serial, &SerialPort::serialClosed, this, &MainWindow::closedSerial);
    connect(m_serial, &SerialPort::dataEmit, this, &MainWindow::receptionData);

}

void MainWindow::handleErrorShow(QString error)
{
   QMessageBox::critical(this, QString("Critical Error"), error);
}


/* MainWindow Information */

void MainWindow::about(){
    QString textAbout;
    textAbout.asprintf("Serial Interface v2/v3 Interface\nVersion : %.1f", VERSION_SERIAL);
    QMessageBox::about(this,"About", textAbout);
}

void MainWindow::showStatusMessage(const QString &stringConnection)
{
    QString message;

    if (stringConnection != "" && stringConnection != *m_connection)
    {
        *m_connection = stringConnection;
    }

    message = QString("%1").arg(*m_connection);

    m_status->setText(message);
}

/* Functions settings systems */

void MainWindow::settingShow(){
    m_settings->show();
}

void MainWindow::setSerialSettings() {
    emit setSerialSettingsSig(m_settings->settings());
}

SerialPort::Settings MainWindow::getSerialInfo() {
    return m_serial->settingsInfo();
}

/* Function open/close serial */

void MainWindow::opennedSerial(SerialPort::Settings p) {
    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss") << "][MAINWINDOW] Serial openned";
    m_serialRun = true;
    showStatusMessage(QString("Connected to %1 : %2, %3, %4, %5, %6")
                      .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                      .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));

    activateButtonSerial();

}

void MainWindow::closedSerial() {
    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss")<< "][MAINWINDOW] Serial closed";
    m_serialRun = false;
    showStatusMessage(QString("Disconnected"));

    disactivateButtonSerial();
}

void MainWindow::openSerialPort() {

    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss")<< "][MAINWINDOW] Send Serial open";
    m_serial->setSerialRun(true);

}

void MainWindow::closeSerialPort() {
    qDebug() << "[" << QDateTime::currentDateTime().toString("dd-MM-yyyy_HH.mm.ss")<< "][MAINWINDOW] Send Serial close";
    m_serial->setSerialRun(false);
}

void MainWindow::activateButtonSerial(){

    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->actionConfigure->setEnabled(false);

}

void MainWindow::disactivateButtonSerial(){

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);

}

void MainWindow::receptionData(bool receptionChek, const QByteArray byteArr){

    if (receptionChek){
        //GPS

        m_data.gpsVar.latitude_mdeg  = ( (byteArr[1] << 24)
            + (byteArr[2] << 16)
            + (byteArr[3] << 8)
            + (byteArr[4] ) );

        m_data.gpsVar.longitude_mdeg = ( (byteArr[5] << 24)
            + (byteArr[6] << 16)
            + (byteArr[7] << 8)
            + (byteArr[8] ) );

        m_data.gpsVar.altitulde_mm = ( (byteArr[9] << 24)
            + (byteArr[10] << 16)
            + (byteArr[11] << 8)
            + (byteArr[12] ) );

        m_data.gpsVar.hourGPS = byteArr[13];
        m_data.gpsVar.minuteGPS = byteArr[14];
        m_data.gpsVar.secondGPS = byteArr[15];
        m_data.gpsVar.gpsRun = byteArr[16];

        //BMP280

        int bmp280Temp =  ( (byteArr[17] << 24)
                + (byteArr[18] << 16)
                + (byteArr[19] << 8)
                + (byteArr[20] ) );

        m_data.bmp280Var.temp = (float) bmp280Temp / 1000.000;

        int bmp280Pres =  ( (byteArr[21] << 24)
                + (byteArr[22] << 16)
                + (byteArr[23] << 8)
                + (byteArr[24] ) );

        m_data.bmp280Var.pres = (float) bmp280Pres / 1000.000;

        m_data.bmp280Var.bmp280Run = byteArr[25];

        //DS18B20

        int ds18b20Temp =  ( (byteArr[26] << 24)
                + (byteArr[27] << 16)
                + (byteArr[28] << 8)
                + (byteArr[29] ) );

        m_data.ds18b20Var.ds18B20Run = (float) ds18b20Temp / 1000.000 ;

        //MPU6050

        int gyroX = ( (byteArr[30] << 24)
                + (byteArr[31] << 16)
                + (byteArr[32] << 8)
                + (byteArr[33] ) );

        m_data.mpu6050Var.gyroX = (float) gyroX / 1000.000;

        int gyroY = ( (byteArr[34] << 24)
                + (byteArr[35] << 16)
                + (byteArr[36] << 8)
                + (byteArr[37] ) );

        m_data.mpu6050Var.gyroY = (float) gyroY / 1000.000;

        int gyroZ = ( (byteArr[38] << 24)
                + (byteArr[39] << 16)
                + (byteArr[40] << 8)
                + (byteArr[41] ) );

        m_data.mpu6050Var.gyroY = (float) gyroZ / 1000.000;

        int AccX  = ( (byteArr[42] << 24)
                + (byteArr[43] << 16)
                + (byteArr[44] << 8)
                + (byteArr[45] ) );

        m_data.mpu6050Var.AccX  = (float) AccX  / 1000.000;

        int AccY  = ( (byteArr[46] << 24)
                + (byteArr[47] << 16)
                + (byteArr[48] << 8)
                + (byteArr[49] ) );

        m_data.mpu6050Var.AccY  = (float) AccY / 1000.000;

        int AccZ  = ( (byteArr[50] << 24)
                + (byteArr[51] << 16)
                + (byteArr[52] << 8)
                + (byteArr[53] ) );

        m_data.mpu6050Var.AccZ  = (float) AccZ / 1000.000;

        m_data.mpu6050Var.mpu6050Run = byteArr[54];

        //DS3231S

        m_data.ds3231Var.secondRTC = byteArr[55];
        m_data.ds3231Var.minuteRTC = byteArr[56];
        m_data.ds3231Var.hourRTC = byteArr[57];
        m_data.ds3231Var.ds3231sRun = byteArr[58];

        //SEQ

        m_data.seqMes = byteArr[59];

    }

}
