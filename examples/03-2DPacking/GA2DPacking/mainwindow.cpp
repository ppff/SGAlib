#include "mainwindow.h"
#include "ui_mainwindow.h"

//Initialize the Random class
INIT_RANDOM()

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //Setup UI
    ui->setupUi(this);

    running = false;
    connect(ui->start, SIGNAL(released()), this, SLOT(buttonClicked()));
    connect(&update, SIGNAL(timeout()), this, SLOT(updateView()));

    //Setup GA
    algorithm.setMainParameters(200, 0.4);
    algorithm.setChromosomesSize(algorithm.numberOfRectangles(), algorithm.numberOfRectangles());
    algorithm.setEndingCriterion(SGA::EndingCriterion::NeverStop);
    algorithm.setSelectionType(SGA::SelectionType::Tournament, 5); //We use tournament because the negative scores behave badly with fitness proportionate selection.
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::buttonClicked()
{
    if (running)
    {
        algorithm.stop();
        update.stop();
        ui->start->setText("Start");
        updateView();
    }
    else
    {
        algorithm.run(false, true, std::cout);
        update.start(20);
        ui->start->setText("Stop");
        ui->rectangles->setText(QString::number(algorithm.numberOfRectangles()));
    }

    running = !running;
}

void MainWindow::updateView()
{
    //Best individual
    const SGA::Chromosome<Gene> best = algorithm.best();

    //Bounding box
    const BB bb = algorithm.boundingBox(best);
    const int width = bb.second.x() - bb.first.x();
    const int height = bb.second.y() - bb.first.y();

    //Painter
    ui->painter->draw(algorithm.getRectangles(best), QRect(bb.first.x(), bb.first.y(), width, height));

    //UI
    ui->generation->setText(QString::number(algorithm.getNumberOfGenerations()));
    ui->collisions->setText(QString::number(algorithm.collisions(best)));
    ui->bbsize->setText(QString::number(width)+QString(" x ")+QString::number(height));
}
