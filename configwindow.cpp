#include "configwindow.h"
#include "ui_configwindow.h"

ConfigWindow::ConfigWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::ConfigWindow),
	refresh_timer(new QTimer(this)),
	current_drives(QList<QStorageInfo>())
{
	ui->setupUi(this);
	QVBoxLayout* layout_drives = new QVBoxLayout(ui->scrollAreaWidgetContents);
	ui->scrollAreaWidgetContents->setLayout(layout_drives);

	refresh_drives();

	QObject::connect(	refresh_timer,	&QTimer::timeout,
						this,			&ConfigWindow::refresh_drives);
	refresh_timer->start(1000);
}

ConfigWindow::~ConfigWindow()
{
	delete refresh_timer;
	delete ui;
}

void ConfigWindow::refresh_drives()
{
	QList<QStorageInfo> read_list = QStorageInfo::mountedVolumes();
	bool isNotChanged = true;
	if (read_list.size() != current_drives.size()) {
		isNotChanged = false;
	} else {
		// TODO: Sort `read_list` first?
		for (int i=0; i<read_list.size(); i++) {
			if (read_list[i] != current_drives[i]) {
				isNotChanged = false;
				break;
			}
		}
	}
	if (!isNotChanged) {
		current_drives = read_list;
		QWidget* button_parent = ui->scrollAreaWidgetContents;
		QLayout* drives_layout = button_parent->layout();
		for (int i=0, size=list_buttons.size(); i<size; i++) {
			drives_layout->removeWidget(list_buttons.back());
			delete list_buttons.back();
			list_buttons.pop_back();
		}
		for (int i=0; i<current_drives.size(); i++) {
			QPushButton* drive_button = new QPushButton(button_parent);
			drive_button->setMinimumHeight(72);
			QString button_text = "";
			QTextStream text_stream(&button_text);
			text_stream << "(" + current_drives[i].rootPath() + ") ";
			if (current_drives[i].rootPath() != current_drives[i].displayName()) {
				text_stream << current_drives[i].displayName() << endl;
			} else {
				text_stream << "[---]" << endl;
			}
			float gigabytes = static_cast<float>(current_drives[i].bytesTotal());
			gigabytes /= 1024.0*1024.0*1024.0;
			text_stream.setNumberFlags(QTextStream::ForcePoint);
			text_stream.setRealNumberPrecision(4);
			text_stream << gigabytes << " GB" << endl;
			QString file_system = current_drives[i].fileSystemType();
			if (file_system == "") {
				text_stream << "[---]";
			} else {
				text_stream << file_system;
			}
			text_stream.flush();
			drive_button->setText(*(text_stream.string()));
			drive_button->setCheckable(true);
			drives_layout->addWidget(drive_button);
			list_buttons.push_back(drive_button);
		}
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
