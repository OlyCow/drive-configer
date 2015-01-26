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
	layout_drives->setContentsMargins(4, 4, 16, 4);
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

void ConfigWindow::on_checkBox_adHoc_toggled(bool checked)
{
	if (checked) {
		if (ui->radioButton_WPA->isChecked() || ui->radioButton_WPA2->isChecked()) {
			ui->radioButton_open->click();
		}
		ui->radioButton_WPA->setDisabled(true);
		ui->radioButton_WPA2->setDisabled(true);
	} else {
		ui->radioButton_WPA->setEnabled(true);
		ui->radioButton_WPA2->setEnabled(true);
	}
}

void ConfigWindow::on_radioButton_open_clicked()
{
	ui->radioButton_disabled->setEnabled(true);
	ui->radioButton_WEP->setEnabled(true);
	if (ui->radioButton_AES->isChecked() || ui->radioButton_TKIP->isChecked()) {
		ui->radioButton_disabled->click();
	}
	ui->radioButton_AES->setDisabled(true);
	ui->radioButton_TKIP->setDisabled(true);
}
void ConfigWindow::on_radioButton_shared_clicked()
{
	ui->radioButton_WEP->setEnabled(true);
	ui->radioButton_WEP->click();
	// It's faster to just uncheck the buttons instead of checking the state
	// of the radio button first (because that requires a call as well).
	ui->radioButton_disabled->setDisabled(true);
	ui->radioButton_AES->setDisabled(true);
	ui->radioButton_TKIP->setDisabled(true);
}
void ConfigWindow::on_radioButton_WPA_clicked()
{
	ui->radioButton_AES->setEnabled(true);
	ui->radioButton_TKIP->setEnabled(true);
	if (ui->radioButton_disabled->isChecked() || ui->radioButton_WEP->isChecked()) {
		ui->radioButton_AES->click();
	}
	ui->radioButton_disabled->setDisabled(true);
	ui->radioButton_WEP->setDisabled(true);
}
void ConfigWindow::on_radioButton_WPA2_clicked()
{
	ui->radioButton_AES->setEnabled(true);
	ui->radioButton_TKIP->setEnabled(true);
	if (ui->radioButton_disabled->isChecked() || ui->radioButton_WEP->isChecked()) {
		ui->radioButton_AES->click();
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

void ConfigWindow::on_pushButton_select_all_clicked()
{
	for (int i=0; i<list_buttons.size(); i++) {
		if (list_buttons[i]->isChecked() == false) {
			list_buttons[i]->click();
		}
	}
}
void ConfigWindow::on_pushButton_select_invert_clicked()
{
	for (int i=0; i<list_buttons.size(); i++) {
		list_buttons[i]->click();
	}
}
void ConfigWindow::on_pushButton_select_none_clicked()
{
	for (int i=0; i<list_buttons.size(); i++) {
		if (list_buttons[i]->isChecked() == true) {
			list_buttons[i]->click();
		}
	}
}

void ConfigWindow::on_pushButton_load_clicked()
{
}
void ConfigWindow::on_pushButton_save_clicked()
{
	QVector<QPushButton*> list_checked;
	QVector<int> index_checked;
	for (int i=0; i<list_buttons.size(); i++) {
		if (list_buttons[i]->isChecked()) {
			list_checked.push_back(list_buttons[i]);
			index_checked.push_back(i);
		}
	}
	for (int i=0; i<list_checked.size(); i++) {
		QString output = "";
		QTextStream text(&output);
		text << "Wireless Network Settings" << endl;
		text << endl;
		text <<	"Print this document and store it in a safe place " <<
				"for future reference.  You may need these settings " <<
				"to add additional computers and devices to your network.";
		text << endl << endl << endl;
		text << "Wireless Settings" << endl;
		text << endl;
		text << "Network Name (SSID): " << get_SSID() << endl;
		text << "Network Key (WEP/WPA Key): " << get_key() << endl;
		text << "Key Provided Automatically (802.1x): " << get_auto_key() << endl;
		text << "Network Authentication Type: " << get_auth_type() << endl;
		text << "Data Encryption Type: " << get_encrypt_type() << endl;
		text << "Connection Type: " << get_connect_type() << endl;
		text << "Key Index: " << get_key_index() << endl;
		text << endl;
		text <<	"To enable File and Printer Sharing on this computer, " <<
				"run the Network Setup Wizard." << endl << endl;
		text <<	"To set up your Internet connection, follow the " <<
				"instructions from your Internet Service Provider (ISP).";
		text << endl;
		text.flush();
		QDir explorer(current_drives[index_checked[i]].rootPath());
		if (ui->checkBox_practice->isChecked()) {
			explorer.mkdir("SMRTNTKY");
			explorer.cd("SMRTNTKY");
			QFile settings(explorer.absolutePath() + "/WSETTING.TXT");
			settings.open(QIODevice::ReadWrite | QIODevice::Text);
			QTextStream writer(&settings);
			writer << output;
			writer.flush();
			settings.close();
			explorer.cdUp();
		}
		if (ui->checkBox_competition->isChecked()) {
			explorer.mkdir("FTCNTKY");
			explorer.cd("FTCNTKY");
			QFile settings(explorer.absolutePath() + "/WSETTING.TXT");
			settings.open(QIODevice::ReadWrite | QIODevice::Text);
			QTextStream writer(&settings);
			writer << output;
			writer.flush();
			settings.close();
			explorer.cdUp();
		}
	}
}

QString ConfigWindow::get_SSID()
{
	return ui->lineEdit_SSID->text();
}
QString ConfigWindow::get_key()
{
	QString master_key = "";
	if (ui->radioButton_WEP->isChecked()) {
	} else if (ui->radioButton_AES->isChecked()) {
		master_key = PBKDF2(	ui->lineEdit_password->text(),
								ui->lineEdit_SSID->text(),
								4096,
								256);
	}
	return master_key;
}
QString ConfigWindow::get_auto_key()
{
	return "0"; // I think this is always "0"?
}
QString ConfigWindow::get_auth_type()
{
	QString auth_type = "";
	if (ui->radioButton_open->isChecked()) {
		auth_type = "Open";
	} else if (ui->radioButton_shared->isChecked()) {
		auth_type = "Shared";
	} else if (ui->radioButton_WPA->isChecked()) {
		auth_type = "WPAPSK";
	} else {
		auth_type = "WPA2PSK";
	}
	return auth_type;
}
QString ConfigWindow::get_encrypt_type()
{
	QString encrypt_type = "";
	if (ui->radioButton_disabled->isChecked()) {
		encrypt_type = "Disabled";
	} else if (ui->radioButton_WEP->isChecked()) {
		encrypt_type = "WEP";
	} else if (ui->radioButton_AES->isChecked()) {
		encrypt_type = "AES";
	} else {
		encrypt_type = "TKIP";
	}
	return encrypt_type;
}
QString ConfigWindow::get_connect_type()
{
	QString connect_type = "";
	if (ui->checkBox_adHoc->isChecked()) {
		connect_type = "IBSS";
	} else {
		connect_type = "ESS";
	}
	return connect_type;
}
int ConfigWindow::get_key_index()
{
	return ui->spinBox_key_index->value();
}

QString ConfigWindow::PBKDF2(QString password, QString salt, int iterations, int length)
{
	return "LETMEIN";
}
