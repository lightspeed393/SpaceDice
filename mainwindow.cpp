#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "functions.h"
#include <QDebug>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("KMDICE Remote GUI");
    //this->statusBar()->setToolTip("Let's play ...");
    this->setMaximumWidth(730);
    this->setMaximumHeight(520);
    this->setMinimumWidth(730);
    this->setMinimumHeight(520);

    ui->lcdBalance->setAutoFillBackground(true);// see the different if you comment that line out.
    QPalette Pal = ui->lcdBalance->palette();
    Pal.setColor(QPalette::Normal, QPalette::WindowText, Qt::green);
    Pal.setColor(QPalette::Normal, QPalette::Window, Qt::black);
    ui->lcdBalance->setPalette(Pal);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_refreshButton_clicked()
{
    t_getinfo res = getInfo();
    ui->lcdBalance->display(res.balance);
    ui->numberOfBlocks->setText(QString::number(res.blocks));
    ui->longestChain->setText(QString::number(res.longestchain));
}

void MainWindow::on_refreshButton_2_clicked()
{

    t_diceinfo res = getDiceInfo();
    //ui->logWindow->append(msg);
    ui->minBet->setText(QString::number(res.minbet, 'f', 8));
    ui->maxBet->setText(QString::number(res.maxbet, 'f', 8));
    ui->maxOdds->setText(QString::number(res.maxodds));
    ui->betAmount->setText(QString::number(res.minbet));
    ui->oddsAmount->setText(QString::number(1));
}

void MainWindow::on_betButton_clicked()
{
    char rawtx[32768], *p_rawtx;
    char txid[65];
    double amount; uint64_t odds;
    int bet_result; double won;

    struct BetThread : public QThread { using QThread::msleep;};

    //ui->betButton->setEnabled(false);
    //QCoreApplication::processEvents();

    QString amount_str=ui->betAmount->text();
    QString odds_str=ui->oddsAmount->text();

    amount = amount_str.toDouble();
    odds = odds_str.toInt();

    t_diceinfo res = getDiceInfo();
    p_rawtx = dicebet(rawtx, sizeof(rawtx), res.name,res.fundingtxid, amount, odds);
    if (p_rawtx) {
        // now we able to sendrawtx
        // qDebug() << p_rawtx;
        sendrawtx(txid, rawtx, strlen(rawtx));
        // qDebug() << txid;
        // ui->logWindow->append(QString("<b>txid: </b>") + QString("<a href=\"http://kmdice.explorer.dexstats.info/tx/") + QString(txid) + QString("\">") + QString(txid) + QString("</a>"));
        ui->logWindow->append(QString("<b>txid: </b>") + QString(txid));

        for (int i = 0; i<5; i++) {
            //ui->MainWindow->statusBar()->showMessage("Bet in progress ["+QString::number(5-i)+"]");
            ui->statusBar->showMessage("Bet in progress ["+QString::number(5-i)+"]");
            //sleep(1);
            BetThread::msleep(1000);

        }
        ui->statusBar->showMessage("");

        bet_result = dicestatus(txid, &won, res.name, res.fundingtxid);
        switch (bet_result) {
        case 1:
            ui->logWindow->append(QString("<font color=\"green\"><b>WIN</b></font> - ") + QString::number(won, 'f', 8));
            break;
        case 0:
            ui->logWindow->append(QString("<font color=\"red\"><b>LOSS</b></font>"));
            break;
        case 2:
            ui->logWindow->append(QString("<b>PENDING</b>"));
            break;
        default:
            ui->logWindow->append(QString("<b>ERROR</b>"));
            break;
        }

        //ui->betButton->setEnabled(true);
        //QCoreApplication::processEvents();
    }


}
