
#include <QtWidgets>
#include <QApplication>
#include <QLabel>

#include <CCamera.h>

int main(int argc, char* argv[]){
  QApplication app(argc, argv);
  QLabel* label = new QLabel("Hellp qt");
  CCamera::getInstance().init();

  
  label->show();
  return app.exec();
}
