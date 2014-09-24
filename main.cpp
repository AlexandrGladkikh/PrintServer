#include <QApplication>
#include <QPrinter>
#include <QPainter>
#include <QRect>
#include <QTextStream>
#include <QString>
#include <fstream>
#include "lib/unp.h"
#include "lib/error.h"
#include "lib/sock.h"
#include "lib/wrap_func.h"

QTextStream cout(stdout);
QTextStream cin(stdin);

struct Setting
{
    int x, y, w, h;
    char addressImg[100];
    Qt::GlobalColor color;

    char font[100];
    int sizeFont;

    char IP[20];
    char port[10];
};

struct SettingData
{
    Setting setting;

    SettingData();
    void Set();
    bool Load();
    QRect GetRect();
    const char* GetAddress();
    const char* GetFont();
    const char* GetIP();
    const char* GetPort();
    int GetFontSize();
    int GetColor();
};

SettingData::SettingData()
{
    setting.x = 0;
    setting.y = 0;
    setting. w = 0;
    setting.h = 0;
}


const char* SettingData::GetIP()
{
    return setting.IP;
}

const char* SettingData::GetPort()
{
    return setting.port;
}

QRect SettingData::GetRect()
{
    return QRect(setting.x, setting.y, setting.w, setting.h);
}

const char* SettingData::GetAddress()
{
    return setting.addressImg;
}

const char* SettingData::GetFont()
{
    return setting.font;
}

int SettingData::GetFontSize()
{
    return setting.sizeFont;
}

int SettingData::GetColor()
{
    return setting.color;
}

void SettingData::Set()
{
    cout << "Enter coordinates and size window:\nx = ";
    cout.flush();

    cin >> setting.x;

    cout << "\ny = ";
    cout.flush();

    cin >> setting.y;

    cout << "\nweight = ";
    cout.flush();

    cin >> setting.w;

    cout << "\nheight = ";
    cout.flush();

    cin >> setting.h;

    cout << "\naddress image = ";
    cout.flush();

    cin >> setting.addressImg;

    cout << "\nfont = ";
    cout.flush();

    cin >> setting.font;

    cout << "\nsize font = ";
    cout.flush();

    cin >> setting.sizeFont;

    cout << "\ncolor font = ";
    cout.flush();

    int clr;
    cin >> clr;

    setting.color = (Qt::GlobalColor)clr;

    cout << "\IP = ";
    cout.flush();

    cin >> setting.IP;

    cout << "\nport = ";
    cout.flush();

    cin >> setting.port;

    std::fstream sttng;

    sttng.open("setting.DAT", std::ios::out | std::ios::binary);

    sttng.write(reinterpret_cast<char*>(&setting), sizeof(setting));

    if (!sttng)
    {
        cout << "\n error open setting";
        exit(1);
    }
}

bool SettingData::Load()
{
    std::fstream sttng;

    sttng.open("setting.DAT", std::ios::in | std::ios::binary);

    sttng.read(reinterpret_cast<char*>(&setting), sizeof(setting));

    if (!sttng)
    {
        sttng.close();
        cout << "\n error open setting set setting:\n";
        return false;
    }
    sttng.close();

    return true;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int				listenfd, connfd;
    socklen_t		addrlen;
    char buff[MAXLINE];
    SettingData setting;

    char ch;

    cout << "Set setting?(y/n): ";
    cout.flush();
    while(1)
    {
        cin >> ch;
        if (ch == 'n')
        {
            if (!setting.Load())
            {
                setting.Set();
            }
            break;
        }
        else if (ch == 'y')
        {
            setting.Set();
            break;
        }
        else
        {
            cout << "Error value";
            continue;
        }
    }

    QPrinter printer;
    QPainter painterPrint(&printer);

    listenfd = Tcp_listen(setting.GetIP(), setting.GetPort(), &addrlen);

    for ( ; ; ) {

        QPixmap image(setting.GetAddress());
        QPainter painterImg(&image);

        painterImg.setPen(setting.GetColor());
        painterImg.setFont(QFont(setting.GetFont(), setting.GetFontSize()));

        connfd = Accept(listenfd, NULL, NULL);

        Read(connfd, buff, strlen(buff));

        painterImg.drawText(setting.GetRect(), Qt::AlignLeft | Qt::AlignTop, buff);

        painterPrint.drawPixmap(setting.GetRect(), image);

        painterImg.end();

        snprintf(buff, MAXLINE, "successful");
        Write(connfd, buff, strlen(buff));

        Close(connfd);
    }

    painterPrint.end();

    return 0;
}




