#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
 
int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  qDebug() << "Hello CMake";
  return a.exec();
}