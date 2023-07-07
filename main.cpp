#include "maindialog.h"

#include <QApplication>
#include"ckernel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
 //   MainDialog w;
 //   w.show();
 //   CKernel* m_pKernel =
    CKernel::GetInstance();
    return a.exec();
}
