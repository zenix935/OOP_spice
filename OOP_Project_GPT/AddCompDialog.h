#pragma once
#include "ui_AddCompDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AddCompDialog; };
QT_END_NAMESPACE

class AddCompDialog : public QDialog,public Ui::AddCompDialog
{
	Q_OBJECT
public:
	AddCompDialog(QWidget* parent=nullptr) : QDialog(parent),ui(new Ui::AddCompDialog())
	{
		ui->setupUi(this);
		connect(ui->okButton,&QPushButton::clicked,this,&AddCompDialog::accepted);
		connect(ui->cancelButton,&QPushButton::clicked,this,&AddCompDialog::rejected);
	}
	~AddCompDialog() {delete ui;}
	Ui::AddCompDialog* ui;
};
