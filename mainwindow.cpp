#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "modal_dialog.h"
#include <string>
#include <iostream>
#include <fstream>
#include <QObject>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->scale_slider,   SIGNAL(sliderMoved(int)),   this,   SLOT(scaleImage(int)));
    connect(ui->s_analysis,     SIGNAL(clicked()),          this,   SLOT(applyAnalyze()));
    connect(ui->delete_button,  SIGNAL(clicked()),          this,   SLOT(deleteEllipse()));
    connect(ui->scaling,        SIGNAL(clicked()),          this,   SLOT(startSettingStandrd()));
    connect(ui->destruct,       SIGNAL(clicked()),          this,   SLOT(init()));
    connect(ui->export_button,  SIGNAL(clicked()),          this,   SLOT(export_impl()));
    connect(ui->import_button,  SIGNAL(clicked()),          this,   SLOT(import()));

    connect(ui->view,   SIGNAL(log(QString)),           this,   SLOT(log(QString)));
    connect(ui->view,   SIGNAL(mousePressed(QPoint)),   this,   SLOT(mousePressed(QPoint)));

    connect(ui->show,   SIGNAL(pressed()),  ui->view,   SLOT(switchImage()));
    connect(ui->show,   SIGNAL(released()), ui->view,   SLOT(switchImage()));

    connect(ui->ellipseID,  SIGNAL(valueChanged(int)),  ui->view,   SLOT(emphasisEllipse(int)));
    connect(ui->ellipseID,  SIGNAL(valueChanged(int)),  this,       SLOT(showDiscription(int)));

    ui->thresholdBox->  setMaximum(255);
    ui->thresholdBox->  setMinimum(0);
    ui->maxBox->        setMaximum(1000);
    ui->minBox->        setMaximum(1000);

    init();
    setAcceptDrops(true);
}

void MainWindow::applyAnalyze(){
    int threshold = ui->thresholdBox->value();
    double min =    ui->minBox->value();
    double max =    ui->maxBox->value();

    ui->view->startAnalysis(threshold,min,max);
    ui->ellipseID->setMinimum( 0 );

    if( ui->view->getEllipses().size() != 0){

        ui->ellipseID->setMaximum( ui->view->getEllipses().size()-1 );

        ui->delete_button-> setEnabled(true);
        ui->export_button-> setEnabled(true);
        ui->show->          setEnabled(true);

    }
}

void MainWindow::log(QString str){
    std::cout << str.toStdString() << std::endl;
    ui->log->append(str);
}

void MainWindow::scaleImage(int value){
    static int oldvalue=50;
    ui->view->scaleView(value-oldvalue);
    oldvalue=value;
}

void MainWindow::showDiscription(int id){
    const Ellipse& ellipse = ui->view->getEllipses()[id];
    double major,minor;
    double m_p_p = ui->view->getScale();
    if(ellipse.rect.size.width > ellipse.rect.size.height){
        major = ellipse.rect.size.width * m_p_p;
        minor = ellipse.rect.size.height * m_p_p;
    } else {
        minor = ellipse.rect.size.width * m_p_p;
        major = ellipse.rect.size.height * m_p_p;
    }

    ui->discription->clear();
    ui->discription->append("Area: " + QString::number(ellipse.area));
    ui->discription->append("Location: (" + QString::number(ellipse.rect.center.x) + "," + QString::number(ellipse.rect.center.y) + ")");
    ui->discription->append("Major axis: " + QString::number(major) + "[μm]");
    ui->discription->append("Minor axis: " + QString::number(minor) + "[μm]");
}

void MainWindow::mousePressed(QPoint pos){
    ui->select_point->setText("("+QString::number(pos.x()) + "," + QString::number(pos.y()) + ")");
    if(isEnteringStandard){
        ModalDialog* dialog = new ModalDialog();
        if(dialog->exec() != QDialog::Rejected){
            ui->view->setStandard(pos,dialog->getValue());
            ui->scale->setText(QString::number(ui->view->getScale()));
        }
        isEnteringStandard=false;
    }
    else{
        const auto& ellipses = ui->view->getEllipses();
        for(int i=0; i<ellipses.size(); i++){
            if(ellipses[i].rect.boundingRect().contains(cv::Point(pos.x(),pos.y()))){
                ui->ellipseID->setValue(i);
            }
        }
    }
}

void MainWindow::destructResult(){
    init();
    ui->view->init();
}

bool MainWindow::init(){
    ui->scale_slider->  setValue(50);
    ui->thresholdBox->  setValue(150);
    ui->minBox->        setValue(10);
    ui->maxBox->        setValue(500);
    ui->ellipseID->     setMinimum(0);
    ui->ellipseID->     setMaximum(0);

    ui->scale_slider->  setEnabled(false);
    ui->s_analysis->    setEnabled(false);
    ui->delete_button-> setEnabled(false);
    ui->export_button-> setEnabled(false);
    ui->show->          setEnabled(false);
    ui->scaling->       setEnabled(false);

    ui->view->init();
    return true;
}

void MainWindow::deleteEllipse(){
    int id = ui->ellipseID->value();
    ui->view->deleteEllipse(id);
}


void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasFormat("text/uri-list"))
    {
        e->acceptProposedAction();
    }
    else{
        ui->log->append("ill-formed file format");
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    fileLocation = QFileInfo(e->mimeData()->urls().first().toLocalFile()).absolutePath();
    fileFullPath = e->mimeData()->urls().first().toLocalFile();

    loadImage(e->mimeData()->urls().first().toLocalFile());
}

void MainWindow::loadImage(QString path){
    cv::Mat src = cv::imread(path.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
    if(!src.empty()){
        ui->view->setImg(src);
        ui->s_analysis->    setEnabled(true);
        ui->scaling->       setEnabled(true);
        ui->scale_slider->  setEnabled(true);
    }
    else{
        ui->log->append("Can't load image. \n The format is wrong, or the path is invalid.");
    }
}

void MainWindow::import(){
   QString filename = QFileDialog::getOpenFileName();

   std::fstream filestream( filename.toStdString() );
   std::vector< Ellipse > ellipses;
   double scale;
   const char delimiter = ',';
   std::string str;

   if (!filestream.is_open())
   {
        return;
   }

   std::string buffer;
   std::istringstream streambuffer(buffer);
   std::string token;

   try{

   filestream >> buffer; //read scale
   getline(streambuffer, token, delimiter);
   scale = std::stod(token);

   filestream >> buffer; //ignore data name

   while (!filestream.eof())
   {
       filestream >> buffer;

       Ellipse ellipse;

       if(buffer.empty())break;

       getline(streambuffer, token, delimiter); //ignore id
       getline(streambuffer, token, delimiter);
       ellipse.rect.center.x = std::stoi(token);
       getline(streambuffer, token, delimiter);
       ellipse.rect.center.y = std::stoi(token);
        getline(streambuffer, token, delimiter);
        ellipse.area = std::stod(token);
        getline(streambuffer, token, delimiter);
        ellipse.rect.size.width = std::stod(token);
        getline(streambuffer, token, delimiter);
        ellipse.rect.size.height = std::stod(token);
        getline(streambuffer, token, delimiter);
        ellipse.rect.angle = std::stod(token);

        ellipses.push_back(ellipse);
   }
   }
   catch(std::exception &e){
       ui->log->append("file import error");
       return;
   }
   ui->view->setEllipses(ellipses);
   ui->view->renderAnalyzedImage(ellipses);
}

void MainWindow::export_impl(){
    export_ellipses(true);
}

void MainWindow::export_ellipses(bool isClear){
    const auto& ellipses = ui->view->getEllipses();
    double m_p_p = ui->view->getScale();
    QString basename = QFileInfo(fileFullPath).baseName();
    double major,minor;

    std::ofstream ofs((fileLocation + "/" + basename + ".csv").toStdString());
    ofs << ui->view->getScale() << ",[μm/pixel]" << std::endl;
    ofs << "ParticleID,Location.x,Location.y,Area,width,height,angle" << std::endl;
    for(int i=0; i<ellipses.size(); i++){
        ofs << i << "," << ellipses[i].rect.center.x << "," << ellipses[i].rect.center.y << ","
            <<  ellipses[i].area << "," << ellipses[i].rect.boundingRect().width*m_p_p << ","
            << ellipses[i].rect.boundingRect().height*m_p_p << "," << ellipses[i].rect.angle << std::endl;
    }
    ui->log->append("export as " + (fileLocation + "/" + basename + ".csv"));
}

MainWindow::~MainWindow()
{
    delete ui;
}
