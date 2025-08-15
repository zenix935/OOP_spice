#pragma once
#include <QDialog>
#include "ui_RLCdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RLCdialog; };
QT_END_NAMESPACE

class RLCdialog : public QDialog
{
	Q_OBJECT
public:
	RLCdialog(QWidget* parent=nullptr) : QDialog(parent),ui(new Ui::RLCdialog)
	{
		ui->setupUi(this);
		setWindowTitle("Component Properties");
		connect(ui->okButton,&QPushButton::clicked,this,&RLCdialog::accepted);
		connect(ui->cancelButton,&QPushButton::clicked,this,&RLCdialog::rejected);
	}
	~RLCdialog() { delete ui; }
	void setName(const QString& s) { ui->NameEdit->setText(s); }
	void setValue(const QString& v) { ui->ValueEdit->setText(v); }
	QString name() { return ui->NameEdit->text(); }
	QString value() { return ui->ValueEdit->text(); }
private:
	Ui::RLCdialog* ui;
};