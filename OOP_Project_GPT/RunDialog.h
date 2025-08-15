#pragma once
#include "ui_RunDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RunDialog; };
QT_END_NAMESPACE

class RunDialog : public QDialog
{
	Q_OBJECT
public:
	RunDialog(QWidget* parent=nullptr) : QDialog(parent),ui(new Ui::RunDialog())
	{
		ui->setupUi(this);
		setWindowTitle("Run");
		connect(ui->okButton,&QPushButton::clicked,this,&RunDialog::accepted);
		connect(ui->cancelButton,&QPushButton::clicked,this,&RunDialog::rejected);
	}
	~RunDialog() { delete ui; }
	Ui::RunDialog* ui;
};