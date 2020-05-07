#include "report.h"
#include "ui_report.h"

report::report(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::report)
{
    ui->setupUi(this);
    setGeometry(geometry().x(), geometry().y(), 800, 800);

    QPixmap pic1("/home/nightmareforev/git/Mock_GUI/SampleProject/Assets/nok1.png");
    ui->label->setPixmap(pic1);
    ui->label->setScaledContents( true );
    ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    QPixmap pic2("/home/nightmareforev/git/Mock_GUI/SampleProject/Assets/ok1.png");
    ui->label_2->setPixmap(pic2);
    ui->label_2->setScaledContents( true );
    ui->label_2->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    QPixmap pic3("/home/nightmareforev/git/Mock_GUI/SampleProject/Assets/ok2.png");
    ui->label_3->setPixmap(pic3);
    ui->label_3->setScaledContents( true );
    ui->label_3->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    QPixmap pic4("/home/nightmareforev/git/Mock_GUI/SampleProject/Assets/ok1.png");
    ui->label_4->setPixmap(pic4);
    ui->label_4->setScaledContents( true );
    ui->label_4->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    QPixmap pic5("/home/nightmareforev/git/Mock_GUI/SampleProject/Assets/ok2.png");
    ui->label_5->setPixmap(pic5);
    ui->label_5->setScaledContents( true );
    ui->label_5->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    QPixmap pic6("/home/nightmareforev/git/Mock_GUI/SampleProject/Assets/nok1.png");
    ui->label_6->setPixmap(pic6);
    ui->label_6->setScaledContents( true );
    ui->label_6->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

}

report::~report()
{
    delete ui;
}
