#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QMimeData>
#include <QStyle>
#include <QDebug>
#include <QDrag>
#include "HelperFunctions.h"
#include "ComponentWidget.h"
#include "LabelNetWidget.h"
#include "LabelNetDialog.h"
#include "ui_MainWindow.h"
#include "ComponentItem.h"
#include "AddCompDialog.h"
#include "LabelNetItem.h"
#include "RLCdialog.h"
#include "RunDialog.h"
#include "GridScene.h"
#include "WireItem.h"
#include "AVdialog.h"
#include "Circuit.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; };
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject* watched,QEvent* ev) override;

private slots:
    void on_actionadd_R_triggered();
    void on_actionadd_C_triggered();
    void on_actionadd_L_triggered();
    void on_actionadd_V_triggered();
    void on_actionadd_I_triggered();
    void on_actionadd_Component_triggered();
    void on_actionadd_LabelNet_triggered();
    void on_actionRun_triggered();

private:
    Ui::MainWindow *ui;
    void placeComponent(const QString& type,const QPointF& scenePos);
    void placeLabelNet(const QString& name,const QPointF& scenePos);
    QPointF snapToGrid(const QPointF& p);
    GridScene* scene=nullptr;
    QPushButton* deleteModeButton=nullptr;
    QPushButton* wireModeButton=nullptr;
    QPushButton* exportButton=nullptr;
    bool deleteMode=false;
    bool wireMode=false;
    QPointF pendingWireStart;
    QVector<std::pair<WireItem*,QVector<QPointF>>> wires;
    QVector<std::pair<ComponentItem*,ComponentWidget*>> components;
    QVector<std::pair<LabelNetItem*,LabelNetWidget*>> labelNets;
};

#endif // !MAINWINDOW_H