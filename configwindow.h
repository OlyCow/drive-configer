#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QMainWindow>

#include <QTimer>

#include <QDir>
#include <QFile>
#include <QFileInfo>

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ConfigWindow(QWidget *parent = 0);
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
	Ui::ConfigWindow *ui;
	QTimer* refresh_timer;

	bool isWindows;
};

#endif // CONFIGWINDOW_H
