#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QMainWindow>

#include <QList>

#include <QTimer>

#include <QStorageInfo>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ConfigWindow(QWidget* parent = 0);
	~ConfigWindow();

private slots:
	void refresh_drives();

	void on_radioButton_open_clicked();
	void on_radioButton_shared_clicked();
	void on_radioButton_WPA_clicked();
	void on_radioButton_WPA2_clicked();

	void on_radioButton_disabled_toggled(bool checked);

	void on_pushButton_show_password_pressed();
	void on_pushButton_show_password_released();

private:
	Ui::ConfigWindow* ui;
	QTimer* refresh_timer;
	QFileInfoList current_drives;

	bool isWindows;
};

#endif // CONFIGWINDOW_H
