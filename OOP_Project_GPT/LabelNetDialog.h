#pragma once
#include "ui_LabelNetDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LabelNetDialog; };
QT_END_NAMESPACE

class LabelNetDialog : public QDialog
{
	Q_OBJECT
public:
	LabelNetDialog(QWidget* parent=nullptr) : QDialog(parent),ui(new Ui::LabelNetDialog())
	{
		ui->setupUi(this);
		connect(ui->okButton,&QPushButton::clicked,this,&LabelNetDialog::accepted);
		connect(ui->cancelButton,&QPushButton::clicked,this,&LabelNetDialog::rejected);
	}
	~LabelNetDialog() { delete ui; }
	Ui::LabelNetDialog* ui;
};