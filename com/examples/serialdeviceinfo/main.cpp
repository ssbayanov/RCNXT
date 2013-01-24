
#include <QtCore>
#include <iostream>

#include <serialdeviceinfo.h>

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    SerialDeviceInfo info;
    
    char dn[50]; //device name
    
    while(1) {
        
        cout << "\n Please enter serial device name, specific by OS, \n example: in Windows -> COMn, in GNU/Linux -> /dev/ttyXYZn: ";
        cin >> dn;
    
        info.setName(dn);
        qDebug() << "\nsystemName   : " << info.systemName();
        qDebug() << "systemPath     : " << info.systemPath();
        qDebug() << "subSystem      : " << info.subSystem();
        qDebug() << "locationInfo   : " << info.locationInfo();
        qDebug() << "friendlyName   : " << info.friendlyName();
        qDebug() << "description    : " << info.description();
        qDebug() << "hardwareID     : " << info.hardwareID();
        qDebug() << "vendorID       : " << info.vendorID(); 
        qDebug() << "productID      : " << info.productID();  
        qDebug() << "manufacturer   : " << info.manufacturer() << "\n";  
        qDebug() << "isExists       : " << info.isExists();
        qDebug() << "isEmpty        : " << info.isEmpty();
        qDebug() << "isBusy         : " << info.isBusy() << "\n";
    }
    
    return app.exec();
}












