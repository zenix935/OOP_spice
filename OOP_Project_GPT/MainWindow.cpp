#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),ui(new Ui::MainWindow())
{
    ui->setupUi(this);
    scene=new GridScene(20,this);
    ui->view->setScene(scene);
    ui->view->setDragMode(QGraphicsView::NoDrag);
    ui->view->setAcceptDrops(true);
    ui->view->viewport()->setAcceptDrops(true);
    ui->view->viewport()->installEventFilter(this);

    connect(ui->deleteModeButton,&QPushButton::toggled,this,[this](bool c) 
    { 
        deleteMode=c; 
        if(c) 
            ui->wireModeButton->setChecked(false);
    });
    connect(ui->wireModeButton,&QPushButton::toggled,this,[this](bool c) 
    { 
        wireMode=c; 
        if(c) 
            ui->deleteModeButton->setChecked(false); 
        pendingWireStart=QPointF(); 
    });
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::placeComponent(const QString& type,const QPointF& scenePos)
{
    ComponentWidget* cw=new ComponentWidget(type);
    if(type=="R")
    {
        int componentNum=1;
        cw->value="1k";
        for (auto component:components)
            if(component.second->type()=="R")
				componentNum++;
        cw->name="R"+QString::number(componentNum);
        cw->setText(cw->type()+" "+cw->value+" "+cw->name);
	}
    if(type=="C")
    {
		int componentNum=1;
		cw->value="1u";
		for(auto component:components)
			if(component.second->type()=="C")
				componentNum++;
		cw->name="C"+QString::number(componentNum);
		cw->setText(cw->type()+" "+cw->value+" "+cw->name);
    }
    if(type=="L")
    {
		int componentNum=1;
		cw->value="1m";
		for(auto component:components)
			if(component.second->type()=="L")
				componentNum++;
		cw->name="L"+QString::number(componentNum);
		cw->setText(cw->type()+" "+cw->value+" "+cw->name);
    }
    if(type=="V")
    {
		int componentNum=1;
		cw->value="5";
		for(auto component:components)
			if(component.second->type()=="V")
				componentNum++;
		cw->name="V"+QString::number(componentNum);
		cw->setText(cw->type()+" DC"+cw->value+" "+cw->name);
    }
    if(type=="I")
    {
		int componentNum=1;
		cw->value="5";
		for(auto component:components)
			if(component.second->type()=="I")
				componentNum++;
		cw->name="I"+QString::number(componentNum);
		cw->setText(cw->type()+" DC"+cw->value+" "+cw->name);
    }
    ComponentItem* ci=new ComponentItem(cw);
    ci->setGridSize(scene->gridSize);
    scene->addItem(ci);
    ci->setPos(scenePos-QPointF(cw->width()/2,cw->height()/2));
    ci->snapToGrid();
    components.append(std::make_pair(ci,cw));
    qDebug()<<ci;
    if(type=="R"||type=="L"||type=="C")
    {
        connect(cw,&ComponentWidget::doubleClicked,this,[ci,cw,this]()
            {
                RLCdialog d;
                d.setName(cw->name);
                d.setValue(cw->value);
                if(d.exec()==QDialog::Accepted)
                {
                    for(auto comp:components)
                    {
                        if(comp.second->name==d.name())
                        {
                            QMessageBox::critical(this,"Duplicate Component","A component with this name already exists.");
                            return;
                        }
                    }
                    cw->name=d.name();
                    cw->value=d.value();
                    cw->setText(cw->type()+" "+cw->value+" "+cw->name);
                }
            });
    }
    if(type=="V"||type=="I")
    {
        connect(cw,&ComponentWidget::doubleClicked,this,[ci,cw,this]()
            {
                AVdialog d;
				d.setName(cw->name);
                if(cw->value.isEmpty())
                {
                    d.setTab(1);
					d.setAmplitude(cw->amplitude);
					d.setFrequency(cw->frequency);
					d.setPhase(cw->phase);
				}
				else
				{
					d.setTab(0);
					d.setValue(cw->value);
                }
                if(d.exec()==QDialog::Accepted)
                {
                    for(auto comp:components)
                    {
                        if(comp.second->name==d.Name())
                        {
                            QMessageBox::critical(this,"Duplicate Component","A component with this name already exists.");
                            return;
                        }
                    }
                    if(d.currentTab()==0&&!d.Value().isEmpty())
                    {
                        cw->name=d.Name();
                        cw->value=d.Value();
                        cw->amplitude="";
                        cw->frequency="";
                        cw->phase="";
                        cw->setText(cw->type()+" DC"+cw->value+" "+cw->name);
                    }
                    else if(d.currentTab()==1)
                    {
                        cw->name=d.Name();
                        cw->amplitude=d.Amplitude();
                        cw->phase=d.Phase();
                        cw->frequency=d.Frequency();
                        cw->value="";
                        cw->setText(cw->type()+" AC"+cw->amplitude+" "+cw->name);
                    }
                }

            });
    }
}

void MainWindow::on_actionadd_R_triggered()
{
    QPointF x(200,200);
    placeComponent("R",x);
}

void MainWindow::on_actionadd_C_triggered()
{
    QPointF x(200,200);
    placeComponent("C",x);
}

void MainWindow::on_actionadd_L_triggered()
{
    QPointF x(200,200);
    placeComponent("L",x);
}

void MainWindow::on_actionadd_V_triggered()
{
    QPointF x(200,200);
    placeComponent("V",x);
}

void MainWindow::on_actionadd_I_triggered()
{
    QPointF x(200,200);
    placeComponent("I",x);
}

void MainWindow::on_actionadd_Component_triggered()
{
    AddCompDialog d;
    if(d.exec()==QDialog::Accepted)
    {
        if(d.ui->ResistorRadioButton->isChecked()) { on_actionadd_R_triggered(); return; }
        if(d.ui->CapacitorRadioButton->isChecked()) { on_actionadd_C_triggered(); return; }
        if(d.ui->InductorRadioButton->isChecked()) { on_actionadd_L_triggered(); return; }
        if(d.ui->VoltageSourceRadioButton->isChecked()) { on_actionadd_V_triggered(); return; }
        if(d.ui->CurrentSourceRadioButton->isChecked()) { on_actionadd_I_triggered(); return; }
    }
}

void MainWindow::on_actionRun_triggered()
{
    RunDialog d;
    d.ui->tabWidget->setCurrentIndex(0);
    if(d.exec()==QDialog::Accepted)
    {

    }
}

QPointF MainWindow::snapToGrid(const QPointF& p) 
{
    int g=scene->gridSize;
    qreal x=std::round(p.x()/(qreal)g)*g;
    qreal y=std::round(p.y()/(qreal)g)*g;
    return {x,y};
}

QGraphicsItem* draggedItem=nullptr;
QPointF dragOffset;

bool MainWindow::eventFilter(QObject* watched,QEvent* ev)
{
    if(watched==ui->view->viewport())
    {
        if(ev->type()==QEvent::MouseButtonPress) 
        {
            QMouseEvent* me=static_cast<QMouseEvent*>(ev);
            QPointF scenePt=ui->view->mapToScene(me->pos());
            QGraphicsItem* clickedItem=scene->itemAt(scenePt,ui->view->transform());
            ComponentItem* ci=nullptr;
            if(me->button()==Qt::LeftButton)
            {
                if(wireMode)
                {
                    QPointF snapped=snapToGrid(scenePt);
                    if(pendingWireStart.isNull())
                        pendingWireStart=snapped;
                    else
                    {
                        QVector<QPointF> pts;
                        QPointF a=pendingWireStart; QPointF b=snapped;
                        if(qFuzzyCompare(a.x(),b.x())||qFuzzyCompare(a.y(),b.y()))
                        {
                            pts.append(a);
                            pts.append(b);
                        }
                        else
                        {
                            QPointF elbow(a.x(),b.y());
                            pts.append(a);
                            pts.append(elbow);
                            pts.append(b);
                        }
                        WireItem* wire=new WireItem(pts);
                        scene->addItem(wire);
                        wires.insert(wire,pts);
                        pendingWireStart=QPointF();
                    }
                    return true;
                }
                else if(deleteMode)
                {
                    QGraphicsItem* it=scene->itemAt(scenePt,ui->view->transform());
                    if(it)
                    {
                        for(int i=0;i<components.size();i++)
                            if(components[i].second->pos()==it->pos())
                            {
                                components.erase(components.begin()+i);
                                break;
                            }
                        scene->removeItem(it);
                        delete it;
                    }
                    return true;
                }
                else
                {
                    for(QGraphicsItem* p=clickedItem; p&&!ci; p=p->parentItem())
                        ci=dynamic_cast<ComponentItem*>(p);
                    if(ci&&ci->flags().testFlag(QGraphicsItem::ItemIsMovable))
                    {
                        draggedItem=ci;
                        dragOffset=scenePt-ci->pos();
                        return true;
                    }
                }
            }
            else if(me->button()==Qt::RightButton)
            {
				if(clickedItem)
				{
					for(QGraphicsItem* p=clickedItem; p&&!ci; p=p->parentItem())
						ci=dynamic_cast<ComponentItem*>(p);
					if(ci)
					{
						ci->rotateCW();
						return true;
					}
				}
            }
        }
        if(ev->type()==QEvent::MouseButtonRelease) 
        {
            if(draggedItem) 
            {
                if(auto* ci=dynamic_cast<ComponentItem*>(draggedItem))
                {
                    QMouseEvent* mm=static_cast<QMouseEvent*>(ev);
                    QPointF scenePt=ui->view->mapToScene(mm->pos());
                    draggedItem->setPos(scenePt-dragOffset);
                    ci->snapToGrid();
                }
                draggedItem=nullptr;
                return true;
            }
        }
        /*if(ev->type()==QEvent::MouseButtonDblClick) 
        {
            QMouseEvent* me=static_cast<QMouseEvent*>(ev);
            QPointF scenePt=ui->view->mapToScene(me->pos());
            QGraphicsItem* it=scene->itemAt(scenePt,ui->view->transform());
        }*/

    }
    return QMainWindow::eventFilter(watched,ev);
}