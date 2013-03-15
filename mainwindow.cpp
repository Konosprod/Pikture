#include "mainwindow.h"
#include "ui_mainwindow.h"

/*@TODO:
 *- Actions sur l'image
 *- Lecture des métadonnées des images / Modifications si possibles
 *- Edition de préférences
 *- Mode diaporama
 *- Différentes icônes
 *- PPA ?
 */

/*Fonction qui parse un QList<QByteArray> pour la transformer en QString
 *utilisé pour avoir la liste sous forme de QString de QImageReader().supportedFormats()
 */
QString qsltos(QList<QByteArray>l, QString sep)
{
    QString ret = "";

    for(int i = 0; i < l.length(); i++)
    {
        ret += l.at(i) + sep;
    }

    return ret;
}

/*Fonction qui créer une QStringList qui peut être utilisée en NameFilters pour
 *lister les fichiers supportés par le QImageReader() dans un dossier
 */
QStringList createNameFilters(QList<QByteArray>l)
{
    QStringList ret;

    for(int i = 0; i < l.length(); i++)
    {
        ret << ("*."+l.at(i));
    }

    return ret;
}

mainWindow::mainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainWindow)
{
    ui->setupUi(this);
    sc = new QScrollArea(this);
    m_label = new QLabel("", this);
    m_image = new QImage();
    m_animated = new QMovie(m_label);
    m_current = -1;

    QStringList args = QApplication::arguments();

    if(args.count() > 1)
    {
        loadFileList(args);
    }

    //sc->setWidgetResizable(true);
    sc->setWidget(m_label);
    sc->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    sc->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setCentralWidget(sc);
    this->resize(800, 600);

    setupConnexions();
}


/*Fonction qui intercept l'évènement resize de la fenêtre et ajuste l'image en conséquence*/
void mainWindow::resizeEvent(QResizeEvent * event)
{
    QMainWindow::resizeEvent(event);
    int w = centralWidget()->width();
    int h = centralWidget()->height();

    /*Debug
    qDebug() << "w : " << (float)m_image->width()/w;
    qDebug() << "h : " << (float)m_image->height()/h;
    */

    if(!m_files.isEmpty())
    {
        QFileInfo fi(m_files.at(m_current));
        if((fi.completeBaseName() != ".gif") && (fi.completeSuffix() != ".GIF"))
        {
            if(((float)m_image->width()/w > 1.0) && ((float)m_image->height()/h > 1.0))
            {
                m_label->setPixmap(QPixmap::fromImage(m_image->scaled(QSize(w, h), Qt::KeepAspectRatioByExpanding)));
                m_label->adjustSize();
            }
            else
            {
                m_label->setPixmap(QPixmap::fromImage(*m_image));
                m_label->adjustSize();
            }
        }
    }
}

/*Fonction qui permet de charger les images passées en argument au programme
 *et qui si elle contient un dossier, ajoute les images contenue dans le dossier
 *à la liste des images à lire
 */
void mainWindow::loadFileList(QStringList l)
{
    /*Debug
    QString ret = "";
    foreach(QString s, l)
    {
        ret+=s+"\n";
    }

    QMessageBox::information(this, "loadFileList", ret);
    */

    for(int i = 1; i < l.count(); i++)
    {
        //Permet de savoir si le fichier existe ou pas et si c'est un dossier ou pas
        QFileInfo* info = new QFileInfo(l.at(i));

        if(info->exists())
        {
            if(!info->isDir())
            {
                m_files << l.at(i);
            }
            else
            {
                m_files += getImagesDir(l.at(i));
            }
        }

        info->~QFileInfo();
    }

    m_current = 0;
    /*Debug
    QString ret = "";
    foreach(QString s, m_files)
    {
        ret+=s+"\n";
    }

    QMessageBox::information(this, "loadFileList", ret);
    */

    drawImage();
}


/*Fonction qui permet d'intercepter un évènement clavier. Ici elle permet de naviguer dans la liste des images
 *passées en paramètres au programme à l'aide des touches X et W
 */
void mainWindow::keyPressEvent(QKeyEvent *event)
{
    if(!m_files.isEmpty())
    {
        if(event->key() == Qt::Key_X)
        {
            if((m_current+1) < m_files.length())
            {
                m_current++;
            }
            else
            {
                m_current = 0;
            }
            drawImage();
            //event->accept();
        }
        else if(event->key() == Qt::Key_W)
        {
            if(m_current-1 >= 0)
            {
            m_current--;
            }
            else
            {
                m_current = m_files.length()-1;
            }

            drawImage();
            //event->accept();
        }
        else
        {
            event->ignore();
        }
    }
}

/*Fonction qui s'occupe d'afficher les images dans le programme*/
void mainWindow::drawImage()
{
    QFileInfo fi(m_files.at(m_current));

    if((fi.completeBaseName() != ".gif") && (fi.completeBaseName() != ".GIF"))
    {
        m_image->~QImage();
        m_image = new QImage();
        if(m_image->load(m_files.at(m_current)))
        {
            m_label->clear();
            m_label->setPixmap(QPixmap::fromImage(*m_image));
            if(((float)m_image->width()/this->centralWidget()->width() > 1.0) && ((float)m_image->height()/this->centralWidget()->height() > 1.0))
            {
                m_label->setPixmap(QPixmap::fromImage(m_image->scaled(QSize(this->centralWidget()->width(), this->centralWidget()->height()), Qt::KeepAspectRatioByExpanding)));
                m_label->adjustSize();
            }
            else
            {
                m_label->setPixmap(QPixmap::fromImage(*m_image));
                m_label->adjustSize();
            }
        }
        else
        {
            QMessageBox::critical(this, "drawImage()", "Impossible d'ouvrir le fichier : " + m_files.at(m_current));
        }

        //Emet le signal qui indique qu'une nouvelle image a été chargée
        emit imageChanged();
    }
    else
    {
        /*GIF en construction
        m_animated->setFileName(m_files.at(m_current));
        m_label->setMovie(m_animated);
        m_label->adjustSize();
        m_animated->start();
        */
    }
}

/*Fonction qui liste les images contenue dans un dossier et en créer un QStringList
 */
QStringList mainWindow::getImagesDir(QString s)
{
    QStringList ret;
    QDir dir(s);

    QStringList tmp = dir.entryList(createNameFilters(QImageReader().supportedImageFormats()), QDir::Files, QDir::Name);

    foreach(QString l, tmp)
    {
        ret << (s+"/"+l);
    }

    return ret;
}


/*Fonction qui est chargée de mettre à jour les différentes informations de la fenêtre
 */
void mainWindow::updateWindow()
{
    QString titleName = "";
    QString mess = ""+ QString::number(m_current+1) + "/" + QString::number(m_files.count());
    ui->statusBar->clearMessage();
    ui->statusBar->showMessage(mess);
    QFileInfo fi(m_files.at(m_current));

    titleName = fi.fileName();

    if(fi.fileName().length() > 60)
    {
        titleName.truncate(57);
        titleName += "...";
    }

    this->setWindowTitle("Pikture - " + titleName);
}

/*Fonction qui permet d'ouvrir des fichiers et de les ajouter à la liste de lecture courante*/
void mainWindow::openFile()
{
    QStringList filters = createNameFilters(QImageReader::supportedImageFormats());
    QString filter = "Image (";
    foreach(QString s, filters)
    {
        filter+=s + " ";
    }
    filter+=")";

    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Ouvrir un fichier", QDir::homePath(), filter);

    if(fileNames.count() > 0)
    {
        m_files += fileNames;
        m_current = m_files.length() - fileNames.length();
        drawImage();
    }
}


/*Fonction qui créer les connexions entres les différents composants
 */
void mainWindow::setupConnexions()
{
    connect(ui->actionDiaporama, SIGNAL(triggered()), this, SLOT(diaporama()));
    connect(ui->actionInformations_fichier, SIGNAL(triggered()), SLOT(fileInfo()));
    connect(ui->actionPr_f_rences, SIGNAL(triggered()), SLOT(preferences());
    connect(ui->actionOuvrir, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(ui->actionQuitter, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(this, SIGNAL(imageChanged()), this, SLOT(updateWindow()));
}

mainWindow::~mainWindow()
{
    delete ui;
    delete m_image;
}
