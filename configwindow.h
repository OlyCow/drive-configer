#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QDebug>

#include <cstdint>
#include <vector>
#include <string>

#include <QVector>
#include <QList>
#include <QByteArray>
#include <QString>
#include <QTextStream>

#include <QMessageAuthenticationCode>

#include <QTimer>

#include <QStorageInfo>
#include <QDir>
#include <QFile>

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>

#include <QMessageBox>
#include <QProgressDialog>

#include <QMainWindow>

#include "aboutwindow.h"

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ConfigWindow(QWidget* parent = 0);
	~ConfigWindow();

	QString get_SSID();
	QString get_key();
	QString get_auto_key();
	QString get_auth_type();
	QString get_encrypt_type();
	QString get_connect_type();
	int get_key_index();

	static QByteArray PBKDF2(QString password, QString salt, int iterations, int length);
	static std::vector<uint8_t> encrypt_block(std::vector<uint8_t> password, std::vector<uint8_t> salt, int iterations, int pass);
	static std::vector<uint8_t> HMAC_SHA1(std::vector<uint8_t> password, std::vector<uint8_t> salt);
	static void disp_char_vect(std::vector<uint8_t> input);

private slots:
	void refresh_drives();

	void on_checkBox_adHoc_toggled(bool checked);

	void on_radioButton_open_clicked();
	void on_radioButton_shared_clicked();
	void on_radioButton_WPA_clicked();
	void on_radioButton_WPA2_clicked();

	void on_radioButton_disabled_toggled(bool checked);

	void on_pushButton_show_password_pressed();
	void on_pushButton_show_password_released();

	void on_lineEdit_password_editingFinished();

	void on_pushButton_select_all_clicked();
	void on_pushButton_select_invert_clicked();
	void on_pushButton_select_none_clicked();

	void on_pushButton_load_clicked();
	void on_pushButton_save_clicked();

	void on_pushButton_about_clicked();

private:
	Ui::ConfigWindow* ui;
	QTimer* refresh_timer;
	QList<QStorageInfo> current_drives;
	QVector<QPushButton*> list_buttons;
	AboutWindow* aboutWindow;
};

#endif // CONFIGWINDOW_H
