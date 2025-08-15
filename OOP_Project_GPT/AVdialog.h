#pragma once
#include "ui_AVdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AVdialog; };
QT_END_NAMESPACE

class AVdialog : public QDialog
{
	Q_OBJECT
public:
	AVdialog(QWidget* parent=nullptr) : QDialog(parent),ui(new Ui::AVdialog())
	{
		ui->setupUi(this);
		setWindowTitle("Component Properties");
		connect(ui->okButton,&QPushButton::clicked,this,&AVdialog::accepted);
		connect(ui->cancelButton,&QPushButton::clicked,this,&AVdialog::rejected);
	}
	~AVdialog() { delete ui; }
	void setTab(int t=0) { ui->tabWidget->setCurrentIndex(t); };
	void setName(const QString& n)
	{
		ui->NameEdit->setText(n);
		ui->NameEdit_2->setText(n);
	}
	void setValue(const QString& v) { ui->ValueEdit->setText(v); }
	void setAmplitude(const QString& a) { ui->AmplitudeEdit_2->setText(a); }
	void setPhase(const QString& p) { ui->PhaseEdit_2->setText(p); };
	void setFrequency(const QString& f) { ui->FrequencyEdit_2->setText(f); };
	int currentTab() { return ui->tabWidget->currentIndex(); }
	QString Name() { return ui->NameEdit->text(); }
	QString Value() { return ui->ValueEdit->text(); }
	QString Amplitude() { return ui->AmplitudeEdit_2->text(); }
	QString Phase() { return ui->PhaseEdit_2->text(); }
	QString Frequency() { return ui->FrequencyEdit_2->text(); }
private:
	Ui::AVdialog* ui;
};