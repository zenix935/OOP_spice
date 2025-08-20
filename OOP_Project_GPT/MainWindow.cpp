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

void MainWindow::placeComponent(const QString& type,const QPointF& scenePos,const QString& name="_none",const QString& value="NAN"
                               ,const QString& phase="NAN",const QString& frequency="NAN")
{
    ComponentWidget* cw=new ComponentWidget(type);
    if(type=="R")
    {
        int componentNum=1;
        if(value=="NAN")
            cw->value="1k";
        else
            cw->value=value;
        if(name=="_none")
        {
            for(auto &component:components)
                if(component.second->type()=="R")
                    componentNum++;
            cw->name="R"+QString::number(componentNum);
        }
        else
            cw->name=name;
        cw->setText(cw->type()+" "+cw->value+" "+cw->name);
	}
    if(type=="C")
    {
		int componentNum=1;
        if(value=="NAN")
            cw->value="1u";
        else
            cw->value=value;
        if(name=="_none")
        {
            for(auto &component:components)
                if(component.second->type()=="C")
                    componentNum++;
            cw->name="C"+QString::number(componentNum);
        }
        else
            cw->name=name;
        cw->setText(cw->type()+" "+cw->value+" "+cw->name);
    }
    if(type=="L")
    {
		int componentNum=1;
        if(value=="NAN")
            cw->value="1m";
        else
            cw->value=value;
        if(name=="_none")
        {
            for(auto &component:components)
                if(component.second->type()=="L")
                    componentNum++;
            cw->name="L"+QString::number(componentNum);
        }
        else
            cw->name=name;
        cw->setText(cw->type()+" "+cw->value+" "+cw->name);
    }
    if(type=="V")
    {
		int componentNum=1;
        if(value=="NAN")
            cw->value="5";
        else
            cw->value=value;
        if(name=="_none")
        {
            for(auto &component:components)
                if(component.second->type()=="V")
                    componentNum++;
            cw->name="V"+QString::number(componentNum);
        }
        else
            cw->name=name;
        if(phase=="NAN"&&frequency=="NAN")
            cw->setText(cw->type()+" DC"+cw->value+" "+cw->name);
        else
        {
            cw->amplitude=value;
            cw->phase=phase;
            cw->frequency=frequency;
            cw->value="";
            cw->setText(cw->type()+" AC"+cw->amplitude+" "+cw->name);
        }
    }
    if(type=="I")
    {
		int componentNum=1;
        if(value=="NAN")
            cw->value="5";
        else
            cw->value=value;
        if(name=="_none")
        {
            for(auto &component:components)
                if(component.second->type()=="I")
                    componentNum++;
            cw->name="I"+QString::number(componentNum);
        }
        else
            cw->name=name;
        if(phase=="NAN"&&frequency=="NAN")
            cw->setText(cw->type()+" DC"+cw->value+" "+cw->name);
        else
        {
            cw->amplitude=value;
            cw->phase=phase;
            cw->frequency=frequency;
            cw->value="";
            cw->setText(cw->type()+" AC"+cw->amplitude+" "+cw->name);
        }
    }
    ComponentItem* ci=new ComponentItem(cw);
    ci->setGridSize(scene->gridSize);
    scene->addItem(ci);
    ci->setPos(scenePos-QPointF(cw->width()/2,cw->height()/2));
    ci->snapToGrid();
    components.append(std::make_pair(ci,cw));
    if(type=="R"||type=="L"||type=="C")
    {
        connect(cw,&ComponentWidget::doubleClicked,this,[ci,cw,this]()
            {
                RLCdialog d;
                d.setName(cw->name);
                d.setValue(cw->value);
                if(d.exec()==QDialog::Accepted)
                {
                    for(auto &comp:components)
                    {
                        if(comp.second->name==d.name()&&comp.second!=cw)
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
                    for(auto &comp:components)
                    {
                        if(comp.second->name==d.Name()&&comp.second!=cw)
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

void MainWindow::placeLabelNet(const QString& name,const QPointF& scenePos)
{
    LabelNetWidget* lnw=new LabelNetWidget(name);
    LabelNetItem* lni=new LabelNetItem(lnw);
    scene->addItem(lni);
    lni->setPos(scenePos-QPointF(lnw->width()/2,lnw->height()/2));
    labelNets.append(std::make_pair(lni,lnw));
    connect(lnw,&LabelNetWidget::doubleClicked,this,[lni,lnw,this]()
        {
            LabelNetDialog d;
            d.ui->LabelNet_edit->setText(lnw->name());
            if(d.exec()==QDialog::Accepted)
            {
                lnw->setName(d.ui->LabelNet_edit->text());
                lnw->setText(lnw->name());
            }
        });
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

void MainWindow::on_actionadd_LabelNet_triggered()
{
    LabelNetDialog d;
    if(d.exec()==QDialog::Accepted)
        placeLabelNet(d.ui->LabelNet_edit->text(),QPointF(200,200));
}

void MainWindow::on_actionRun_triggered()
{
    RunDialog d;
    d.ui->tabWidget->setCurrentIndex(0);
    if(d.exec()==QDialog::Accepted)
    {
        bool isGrounded=false;
        for(auto &label:labelNets)
        {
            std::string Name=label.second->name().toStdString();
            std::transform(Name.begin(),Name.end(),Name.begin(),[](unsigned char c){return std::toupper(c);});
            if(Name=="GND")
            {
                isGrounded=true;
                break;
            }
        }
        if(!isGrounded)
        {
            QMessageBox::critical(this,"No Ground","No ground node founded in the circuit");
            return;
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    QString filter="All Files (*.*);;Text Files (*.txt)";
    QString filename=QFileDialog::getOpenFileName(this,"Choose a file for saving","C:/",filter);
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QMessageBox::critical(this,"Error","Could not open file");
        return;
    }
    QString text;
    for(auto &comp:components)
    {
        text=text+comp.second->type()+" "+comp.second->name+" "+comp.second->value+" "+comp.second->amplitude+" "+comp.second->phase+" "+comp.second->frequency+" ";
        text=text+QString::number(comp.second->pos().x()+40)+" "+QString::number(comp.second->pos().y()+20)+"\n";
    }
    for(auto &wire:wires)
    {
        text=text+"wire ";
        for(auto po:wire.second)
            text=text+QString::number(po.x())+" "+QString::number(po.y())+" ";
        text=text+"\n";
    }
    for(auto &label:labelNets)
        text=text+"label "+label.second->name()+" "+QString::number(label.second->pos().x()+15)+" "+QString::number(label.second->pos().y()+10)+"\n";
    qDebug()<<text;
    QTextStream out(&file);
    out<<text;
    file.flush();
    file.close();
}

void MainWindow::on_actionOpen_triggered()
{
    QString filter="All Files (*.*);;Text Files (*.txt)";
    QString filename=QFileDialog::getOpenFileName(this,"Choose a file for saving","C:/",filter);
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QMessageBox::critical(this,"Error","Could not open file");
        return;
    }
    QString text;
    QVector<QString> lines;
    QTextStream in(&file);
    while(!in.atEnd())
        lines.append(in.readLine());
    file.close();
    components.clear();
    wires.clear();
    labelNets.clear();
    scene->clear();
    QVector<QVector<QString>> tokens;
    for(auto &line:lines)
    {
        QVector<QString> temp;
        QStringList words=line.split(QRegularExpression("\\s+"),Qt::SkipEmptyParts);
        for(const QString& token:words)
            temp.append(token);
        tokens.append(temp);
        temp.clear();
    }
    for(auto &comp:tokens)
    {
        qDebug()<<comp;
        if(comp[0]=="V"||comp[0]=="I"||comp[0]=="L"||comp[0]=="R"||comp[0]=="C")
        {
            if(comp.size()==5)
            {
                QPointF pos(comp[3].toDouble()+1,comp[4].toDouble()+1);
                placeComponent(comp[0],pos,comp[1],comp[2]);
            }
            else if(comp.size()==7)
            {
                QPointF pos(comp[5].toDouble()+1,comp[6].toDouble()+1);
                placeComponent(comp[0],pos,comp[1],comp[2],comp[3],comp[4]);
            }
        }
        if(comp[0]=="wire")
        {
            QVector<QPointF> pts;
            QPointF a(comp[1].toDouble(),comp[2].toDouble()),b(comp[comp.size()-2].toDouble(),comp[comp.size()-1].toDouble());
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
            wires.append(std::make_pair(wire,pts));
        }
        if(comp[0]=="label")
        {
            QPointF pos(comp[2].toDouble(),comp[3].toDouble());
            placeLabelNet(comp[1],pos);
        }
    }
}

void MainWindow::on_actionNew_triggered()
{
    components.clear();
    wires.clear();
    labelNets.clear();
    scene->clear();
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
            LabelNetItem* ln=nullptr;
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
                        wires.append(std::make_pair(wire,pts));
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
                                qDebug()<<it->pos();
                                break;              
                            }
                        for(int i=0;i<wires.size();i++)
                            if(wires[i].first->pos()==it->pos())
                            {
                                wires.erase(wires.begin()+i);
                                qDebug()<<it->pos();
                                break;
                            }
                        for(int i=0;i<labelNets.size();i++)
                            if(labelNets[i].first->pos()==it->pos())
                            {
                                labelNets.erase(labelNets.begin()+i);
                                qDebug()<<it->pos();
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
                    for(QGraphicsItem* p=clickedItem;p&&!ln;p=p->parentItem())
                        ln=dynamic_cast<LabelNetItem*>(p);
                    if(ln&&ln->flags().testFlag(QGraphicsItem::ItemIsMovable))
                    {
                        draggedItem=ln;
                        dragOffset=scenePt-ln->pos();
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
                else if(auto* ln=dynamic_cast<LabelNetItem*>(draggedItem))
                {
                    QMouseEvent* mm=static_cast<QMouseEvent*>(ev);
                    QPointF scenePt=ui->view->mapToScene(mm->pos());
                    draggedItem->setPos(scenePt-dragOffset);
                }
                draggedItem=nullptr;
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched,ev);
}