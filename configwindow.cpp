#include "configwindow.h"
#include "ui_configwindow.h"

ConfigWindow::ConfigWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::ConfigWindow),
	refresh_timer(new QTimer(this)),
	current_drives(QFileInfoList())
{
	ui->setupUi(this);
	ui->scrollArea_drives->setLayout(new QVBoxLayout(this));

	refresh_drives();

	QObject::connect(	refresh_timer,	&QTimer::timeout,
						this,			&ConfigWindow::refresh_drives);
	refresh_timer->start(1000);

	#ifdef Q_OS_WIN
	isWindows = true;
	#else
	isWindows = false;
	#endif //Q_OS_WIN
}

ConfigWindow::~ConfigWindow()
{
	delete ui;
	delete refresh_timer;
}

void ConfigWindow::refresh_drives()
{
	QFileInfoList root_list = QDir::drives();
	if (isWindows) {
		bool isNotChanged = true;
		if (root_list.size() != current_drives.size()) {
			isNotChanged = false;
		} else {
			// TODO: Sort `root_list` first.
			for (int i=0; i<root_list.size(); i++) {
				if (root_list[i] != current_drives[i]) {
					isNotChanged = false;
					break;
				}
			}
		}
		if (!isNotChanged) {
			current_drives = root_list;
			QScrollArea* button_parent = ui->scrollArea_drives;
			QLayout* drives_layout = button_parent->layout();
			QList<QPushButton*> button_list = drives_layout->findChildren<QPushButton*>();
			for (int i=0; i<button_list.size(); i++) {
				drives_layout->removeWidget(button_list.back());
				delete button_list.back();
				button_list.pop_back();
			}
			for (int i=0; i<current_drives.size(); i++) {
				QPushButton* drive_button = new QPushButton(button_parent);
				drive_button->setMinimumHeight(60);
				drive_button->setText(current_drives[i].absolutePath());
				drive_button->setCheckable(true);
				drives_layout->addWidget(drive_button);
			}
		}
	} else {
	}
}

void ConfigWindow::on_radioButton_open_clicked()
{
	ui->radioButton_disabled->setEnabled(true);
	ui->radioButton_WEP->setEnabled(true);
	if (ui->radioButton_AES->isChecked() || ui->radioButton_TKIP->isChecked()) {
		ui->radioButton_disabled->setChecked(true);
		ui->radioButton_AES->setChecked(false);
		ui->radioButton_TKIP->setChecked(false);
	}
	ui->radioButton_AES->setDisabled(true);
	ui->radioButton_TKIP->setDisabled(true);
}

void ConfigWindow::on_radioButton_shared_clicked()
{
	ui->radioButton_WEP->setEnabled(true);
	ui->radioButton_WEP->setChecked(true);
	// It's faster to just uncheck the buttons instead of checking the state
	// of the radio button first (because that requires a call as well).
	ui->radioButton_disabled->setDisabled(true);
	ui->radioButton_disabled->setChecked(false);
	ui->radioButton_AES->setDisabled(true);
	ui->radioButton_AES->setChecked(false);
	ui->radioButton_TKIP->setDisabled(true);
	ui->radioButton_TKIP->setChecked(false);
}

void ConfigWindow::on_radioButton_WPA_clicked()
{
	ui->radioButton_AES->setEnabled(true);
	ui->radioButton_TKIP->setEnabled(true);
	if (ui->radioButton_disabled->isChecked() || ui->radioButton_WEP->isChecked()) {
		ui->radioButton_AES->setChecked(true);
		ui->radioButton_disabled->setChecked(false);
		ui->radioButton_WEP->setChecked(false);
	}
	ui->radioButton_disabled->setDisabled(true);
	ui->radioButton_WEP->setDisabled(true);
}

void ConfigWindow::on_radioButton_WPA2_clicked()
{
	ui->radioButton_AES->setEnabled(true);
	ui->radioButton_TKIP->setEnabled(true);
	if (ui->radioButton_disabled->isChecked() || ui->radioButton_WEP->isChecked()) {
		ui->radioButton_AES->setChecked(true);
		ui->radioButton_disabled->setChecked(false);
		ui->radioButton_WEP->setChecked(false);
	}
	ui->radioButton_disabled->setDisabled(true);
	ui->radioButton_WEP->setDisabled(true);
}

void ConfigWindow::on_radioButton_disabled_toggled(bool checked)
{
	ui->label_password->setDisabled(checked);
	ui->lineEdit_password->setDisabled(checked);
	ui->pushButton_show_password->setDisabled(checked);
	ui->label_key_index->setDisabled(checked);
	ui->spinBox_key_index->setDisabled(checked);
	ui->label_key_index_help->setDisabled(checked);
}

void ConfigWindow::on_pushButton_show_password_pressed()
{
	ui->lineEdit_password->setEchoMode(QLineEdit::Normal);
}
void ConfigWindow::on_pushButton_show_password_released()
{
	ui->lineEdit_password->setEchoMode(QLineEdit::Password);
}
