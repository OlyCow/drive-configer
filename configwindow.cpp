#include "configwindow.h"
#include "ui_configwindow.h"

ConfigWindow::ConfigWindow(QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui::ConfigWindow),
	refresh_timer(new QTimer(this)),
	current_drives(QList<QStorageInfo>()),
	aboutWindow(new AboutWindow(this))
{
	ui->setupUi(this);
	QVBoxLayout* layout_drives = new QVBoxLayout(ui->scrollAreaWidgetContents);
	layout_drives->setContentsMargins(4, 4, 4, 4);
	ui->scrollAreaWidgetContents->setLayout(layout_drives);

	refresh_drives();

	QObject::connect(	refresh_timer,	&QTimer::timeout,
						this,			&ConfigWindow::refresh_drives);
	refresh_timer->start(1000);
}

ConfigWindow::~ConfigWindow()
{
	delete aboutWindow;
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
				text_stream << "[NO NAME]" << endl;
			}
			float bytes = static_cast<float>(current_drives[i].bytesTotal());
			float kilobytes = bytes/1024.0;
			float megabytes = kilobytes/1024.0;
			float gigabytes = megabytes/1024.0;
			enum DispMag {
				DISP_B	= 0,
				DISP_K,
				DISP_M,
				DISP_G
			};
			DispMag dispMag = DISP_G;
			float disp_float = gigabytes;
			if (gigabytes < 1.0) {
				if (megabytes > 1.0) {
					dispMag = DISP_M;
				} else if (kilobytes > 1.0) {
					dispMag = DISP_K;
				} else {
					dispMag = DISP_B;
				}
			}
			switch (dispMag) {
				case DISP_B :
					disp_float = bytes;
					break;
				case DISP_K :
					disp_float = kilobytes;
					break;
				case DISP_M :
					disp_float = megabytes;
					break;
				case DISP_G :
					disp_float = gigabytes;
					break;
			}
			int float_precision = 4;
			if (disp_float < 100.0) {
				float_precision = 3;
			}
			if (bytes >= 1.0) {
				text_stream.setNumberFlags(QTextStream::ForcePoint);
				text_stream.setRealNumberPrecision(float_precision);
			}
			text_stream << disp_float << " ";
			switch (dispMag) {
				case DISP_B :
					text_stream << "bytes";
					break;
				case DISP_K :
					text_stream << "kB";
					break;
				case DISP_M :
					text_stream << "MB";
					break;
				case DISP_G :
					text_stream << "GB";
					break;
			}

			text_stream << endl;
			QString file_system = current_drives[i].fileSystemType();
			if (file_system == "") {
				text_stream << "Unknown FS";
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

void ConfigWindow::on_lineEdit_password_editingFinished()
{
	QString password = ui->lineEdit_password->text();
	int length = password.length();
	QString message = "";
	if (ui->radioButton_AES->isChecked() || ui->radioButton_TKIP->isChecked()) {
		if (length>0) {
			bool isASCII = true;
			for (int i=0; i<length; i++) {
				if (password[i].unicode() > 127) {
					isASCII = false;
					break;
				}
			}
			if (!isASCII) {
				message = "The password must only contain ASCII characters.";
			} else if (length<8 || length>63) {
				if (length == 64) {
					message = "You have entered a hexadecimal password. If you want to enter a normal password, it must be 8 to 63 characters long.";
				} else {
					message = "The password must be 8 to 63 characters long.";
				}
			} else {
			}
		}
	} else if (ui->radioButton_WEP->isChecked()) {
		if (length > 0) {
			bool isASCII = true;
			for (int i=0; i<length; i++) {
				if (password[i].unicode() > 127) {
					isASCII = false;
					break;
				}
			}
			if (!isASCII) {
				message = "The password must only contain ASCII characters.";
			} else if (length == 10 || length == 26) {
				message = "You have entered a hexadecimal password. If you want to enter a normal password, it must be 8 to 63 characters long.";
			} else if (length != 5 && length != 13) {
				message = "The password must be exactly 5 or 13 characters long.";
			}
		}
	}
	ui->label_password_confirm->setText(message);
	ui->label_password_confirm->adjustSize();
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
	QString warning_title = "Warning - Drive Configer";
	QVector<QPushButton*> list_checked;
	QVector<int> index_checked;
	for (int i=0; i<list_buttons.size(); i++) {
		if (list_buttons[i]->isChecked()) {
			list_checked.push_back(list_buttons[i]);
			index_checked.push_back(i);
		}
	}
	if (list_checked.size() == 0) {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Warning,
												warning_title,
												"Please select a drive to load configs from.",
												QMessageBox::Ok,
												this	);
		warn->exec();
		return;
	}
	if (list_checked.size() > 1) {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Warning,
												warning_title,
												"You selected more than one drive. Please select a single drive to load configs from.",
												QMessageBox::Ok,
												this	);
		warn->exec();
		return;
	}
	if (current_drives[index_checked[0]].fileSystemType() != "FAT32") {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Information,
												warning_title,
												"The file system of the selected drive is not \"FAT32\". This drive is not guaranteed to work.",
												QMessageBox::Ok,
												this	);
		warn->exec();
	}
	QDir* practice_path = new QDir(current_drives[index_checked[0]].rootPath());
	QDir* competition_path = new QDir(current_drives[index_checked[0]].rootPath());
	bool isPractice = practice_path->cd("SMRTNTKY");
	bool isCompetition = practice_path->cd("FTCNTKY");
	if ((isPractice || isCompetition) == false) {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Information,
												warning_title,
												"There were no config files detected on the selected drive.",
												QMessageBox::Ok,
												this	);
		warn->exec();
		return;
	}
	if ((isPractice && isCompetition) == true) {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Information,
												warning_title,
												"Both practice and competition configs were detected. Competition configs will overwrite practice ones.",
												QMessageBox::Ok,
												this	);
		warn->exec();
	}
	if (isPractice) {
		ui->checkBox_practice->setChecked(true);
		read_configs(practice_path->absolutePath() + "/WSETTING.TXT");
	} else {
		ui->checkBox_practice->setChecked(false);
	}
	if (isCompetition) {
		ui->checkBox_competition->setChecked(true);
		read_configs(competition_path->absolutePath() + "/WSETTING.TXT");
	} else {
		ui->checkBox_competition->setChecked(false);
	}
}
void ConfigWindow::on_pushButton_save_clicked()
{
	QString warning_title = "Warning - Drive Configer";
	QVector<QPushButton*> list_checked;
	QVector<int> index_checked;
	bool isFAT32 = true;
	for (int i=0; i<list_buttons.size(); i++) {
		if (list_buttons[i]->isChecked()) {
			list_checked.push_back(list_buttons[i]);
			index_checked.push_back(i);
			if (current_drives[i].fileSystemType() != "FAT32") {
				isFAT32 = false;
			}
		}
	}
	if (!isFAT32) {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Information,
												warning_title,
												"One or more of the selected drives is not formatted as FAT32. There is no guarantee that the Samantha will be successfully flashed.",
												QMessageBox::Ok,
												this	);
		warn->exec();
	}
	if (list_checked.size() == 0) {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Warning,
												warning_title,
												"Please select a drive to write the wireless configs to.",
												QMessageBox::Ok,
												this	);
		warn->exec();
		return;
	}
	if (ui->label_password_confirm->text() != "") {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Warning,
												warning_title,
												"The password is invalid. Please check the \"password\" field for more information.",
												QMessageBox::Ok,
												this	);
		warn->exec();
		return;
	}
	if (ui->radioButton_disabled->isChecked() == false && ui->lineEdit_password->text().length() == 0) {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Warning,
												warning_title,
												"You must enter a password for this encryption type.",
												QMessageBox::Ok,
												this	);
		warn->exec();
		return;
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
			settings.open(QIODevice::WriteOnly | QIODevice::Text);
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
			settings.open(QIODevice::WriteOnly | QIODevice::Text);
			QTextStream writer(&settings);
			writer << output;
			writer.flush();
			settings.close();
			explorer.cdUp();
		}
	}
}

void ConfigWindow::read_configs(QString file_path)
{
	QString error_title = "Error - Drive Configer";
	QFile settings(file_path);
	settings.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream text(&settings);
	QString text_buff;
	text_buff = text.readLine();			// "Wireless Network Settings"
	text_buff = text.readLine();			// ""
	text_buff = text.readLine();			// "Print this document [...]"
	text_buff = text.readLine();			// ""
	text_buff = text.readLine();			// ""
	text_buff = text.readLine();			// "Wireless Settings"
	text_buff = text.readLine();			// ""
	QString text_SSID = text.readLine();	// "Network Name (SSID): "
	QString text_key = text.readLine();		// "Network Key (WEP/WPA Key): "
	text_buff = text.readLine();			// "Key Provided Automatically (802.1x): 0"
	QString text_auth = text.readLine();	// "Network Authentication Type: "
	QString text_encrypt = text.readLine();	// "Data Encryption Type: "
	QString text_ad_hoc = text.readLine();	// "Connection Type: "
	QString text_index = text.readLine();	// "Key Index: "
	settings.close();

	text_SSID.remove(0, QString("Network Name (SSID): ").length());
	ui->lineEdit_SSID->setText(text_SSID);
	text_key.remove(0, QString("Network Key (WEP/WPA Key): ").length());
	ui->lineEdit_password->setText(text_key);
	text_auth.remove(0, QString("Network Authentication Type: ").length());
	if (text_auth == "Open") {
		ui->radioButton_open->click();
	} else if (text_auth == "Shared") {
		ui->radioButton_shared->click();
	} else if (text_auth == "WPAPSK") {
		ui->radioButton_WPA->click();
	} else if (text_auth == "WPA2PSK") {
		ui->radioButton_WPA2->click();
	} else {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Critical,
												error_title,
												"Cannot recognize authentication protocol.",
												QMessageBox::Ok,
												this	);
		warn->exec();
	}
	text_encrypt.remove(0, QString("Data Encryption Type: ").length());
	if (text_encrypt == "Disabled") {
		ui->radioButton_disabled->click();
	} else if (text_auth == "WEP") {
		ui->radioButton_WEP->click();
	} else if (text_auth == "AES") {
		ui->radioButton_AES->click();
	} else if (text_auth == "TKIP") {
		ui->radioButton_TKIP->click();
	} else {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Critical,
												error_title,
												"Cannot recognize method of encryption.",
												QMessageBox::Ok,
												this	);
		warn->exec();
	}
	text_ad_hoc.remove(0, QString("Connection Type: ").length());
	if (text_ad_hoc == "IBSS") {
		ui->checkBox_adHoc->setChecked(true);
	} else if (text_ad_hoc == "ESS") {
		ui->checkBox_adHoc->setChecked(false);
	} else {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Critical,
												error_title,
												"Cannot recognize connection type (ad-hoc vs. not).",
												QMessageBox::Ok,
												this	);
		warn->exec();
	}
	int key_index = text_index.remove(0, QString("Key Index: ").length()).toInt();
	if (key_index >= 1 && key_index <= 4) {
		ui->spinBox_key_index->setValue(key_index);
	} else {
		QMessageBox* warn = new QMessageBox(	QMessageBox::Critical,
												error_title,
												"Key index out-of-range.",
												QMessageBox::Ok,
												this	);
		warn->exec();
	}

	qDebug() << text_SSID;
	qDebug() << text_key;
	qDebug() << text_auth;
	qDebug() << text_encrypt;
	qDebug() << text_ad_hoc;
	qDebug() << text_index;
}

void ConfigWindow::on_pushButton_about_clicked()
{
	aboutWindow->show();
}

QString ConfigWindow::get_SSID()
{
	return ui->lineEdit_SSID->text();
}
QString ConfigWindow::get_key()
{
	QString master_key = "";
	QString password = ui->lineEdit_password->text();
	int length = password.length();
	if (ui->radioButton_WEP->isChecked()) {
		if (length == 10 || length == 26) {
			master_key = password;
		} else {
			QByteArray ascii_chars(password.toLatin1().toHex());
			master_key = QString(ascii_chars);
		}
	} else if (ui->radioButton_AES->isChecked() || ui->radioButton_TKIP->isChecked()) {
		if (length == 64) {
			master_key = password;
		} else {
			master_key = PBKDF2(	ui->lineEdit_password->text(),
									ui->lineEdit_SSID->text(),
									4096,
									32);
		} // else it should stay as "" (blank)
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

QByteArray ConfigWindow::PBKDF2(QString password, QString salt, int iterations, int length)
{
	QProgressDialog* encrypt_progress = new QProgressDialog();
	encrypt_progress->setLabelText("Password encryption progress:");
	encrypt_progress->setMaximum(100);
	encrypt_progress->setMinimumDuration(0);
	encrypt_progress->setWindowTitle("Encryption Progress");
	encrypt_progress->setWindowModality(Qt::WindowModal);
	encrypt_progress->setMinimumSize(400, 90);
	encrypt_progress->setValue(0);
	encrypt_progress->show();

	std::vector<uint8_t> output(length);
	std::vector<uint8_t> password_vect;
	for (int i=0; i<password.length(); i++) {
		password_vect.push_back((uint8_t)(password[i].toLatin1()));
	}
	std::vector<uint8_t> salt_vect;
	for (int i=0; i<salt.length(); i++) {
		salt_vect.push_back((uint8_t)(salt[i].toLatin1()));
	}
	std::vector<uint8_t> block_A = encrypt_block(	password_vect,
													salt_vect,
													iterations,
													1);
	encrypt_progress->setValue(50);
	std::vector<uint8_t> block_B = encrypt_block(	password_vect,
													salt_vect,
													iterations,
													2);
	encrypt_progress->setValue(100);
	delete encrypt_progress;
	for (int i=0; i<20; i++) {
		output[i] = block_A[i];
	}
	for (int i=0; i<12; i++) {
		output[i+20] = block_B[i];
	}
	QByteArray output_key;
	for (int i = 0; i<length; i++) {
		output_key.push_back(output[i]);
	}
	return output_key.toHex();
}

std::vector<uint8_t> ConfigWindow::encrypt_block(std::vector<uint8_t> password, std::vector<uint8_t> salt, int iterations, int pass)
{
	// HMAC-SHA1 returns 160 bits (20 bytes)
	std::vector<uint8_t> output;
	int size_salt = salt.size();
	const int size_int = 32/8; // 32-bit int is 4 bytes
	int size_key = size_salt + size_int;
	std::vector<uint8_t> key(size_key);
	for (int i=0; i<size_salt; i++) {
		key[i] = salt[i];
	}
	for (int i=size_int; i>0; i--) {
		uint32_t temp = pass >> (8*(i-1));
		uint32_t output = 0xFF & temp;
		uint8_t char_output = output;
		key[size_salt+(size_int-i)] = char_output;
	}
	std::vector<uint8_t> U_1 = HMAC_SHA1(password, key);

	for (int i=0; i<20; i++) {
		output.push_back(U_1[i]);
	}
	std::vector<uint8_t> U_i(U_1);
	// start at i=1 because already had initial pass
	for (int i=1; i<iterations; i++) {
		U_i = HMAC_SHA1(password, U_i);
		for (int j=0; j<20; j++) {
			output[j] = output[j] ^ U_i[j];
		}
	}
	return output;
}

std::vector<uint8_t> ConfigWindow::HMAC_SHA1(std::vector<uint8_t> password, std::vector<uint8_t> salt)
{
	std::vector<uint8_t> output;
	QByteArray raw_password;
	for (unsigned int i=0; i<password.size(); i++) {
		raw_password.push_back(password[i]);
	}
	QByteArray raw_salt;
	for (unsigned int i=0; i<salt.size(); i++) {
		raw_salt.push_back(salt[i]);
	}
	QByteArray hashed = QMessageAuthenticationCode::hash(	raw_salt,
															raw_password,
															QCryptographicHash::Sha1);
	char* output_raw = hashed.data();
	for (int i=0; i<hashed.length(); i++) {
		uint8_t coal = output_raw[i];
		output.push_back((uint8_t)coal); //this was one line but I thought it was necessary
	}
	return output;
}

void ConfigWindow::disp_char_vect(std::vector<uint8_t> input)
{
	QByteArray disp;
	for (unsigned int i=0; i<input.size(); i++) {
		disp.push_back(input[i]);
	}
	qDebug() << disp.toHex();
}
