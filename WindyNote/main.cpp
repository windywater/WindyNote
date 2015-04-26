#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QFile>
#include "DatabaseService.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator translator;
	translator.load(":/WindyNote/zh.qm");
	a.installTranslator(&translator);

	QFile qssFile(":/WindyNote/style.qss");
	qssFile.open(QIODevice::ReadOnly);
	QString qss = QString::fromUtf8(qssFile.readAll().data());
	qssFile.close();
	a.setStyleSheet(qss);

	DatabaseService::Instance().Init();

	MainWindow w;
	w.show();
	return a.exec();
}
