#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "functions.h"
#include <QDebug>
#include <QThread>
#include <QTime>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Dice in Space");
    //this->statusBar()->setToolTip("Let's play ...");
    this->setMaximumWidth(750);
    this->setMaximumHeight(554);
    this->setMinimumWidth(750);
    this->setMinimumHeight(554);

    ui->lcdBalance->setAutoFillBackground(true);// see the different if you comment that line out.
    QPalette Pal = ui->lcdBalance->palette();
    Pal.setColor(QPalette::Normal, QPalette::WindowText, Qt::green);
    Pal.setColor(QPalette::Normal, QPalette::Window, Qt::black);
    ui->lcdBalance->setPalette(Pal);


    ui->link1->setText("<a href=\"https://spaceworks.co/guides\">Dice Guides</a> | <a href=\"https://explorer.spaceworks.co/\">Explorer</a>");
    ui->link1->setTextFormat(Qt::RichText);
    ui->link1->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->link1->setOpenExternalLinks(true);


    QStringList keys;
    keys << "Time" << "Txid" << "Amount" << "Odds" << "Result";
    ui->tableHistory->clear();
    ui->tableHistory->setRowCount(0);
    ui->tableHistory->setColumnCount(5);
    ui->tableHistory->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableHistory->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableHistory->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableHistory->setHorizontalHeaderLabels(keys);

    ui->refreshButton->setShortcut(QKeySequence(Qt::Key_F5));

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
        // ui->logWindow->append(QString("<b>txid: </b>") + QString("<a href=\"http://space.explorer.dexstats.info/tx/") + QString(txid) + QString("\">") + QString(txid) + QString("</a>"));
        ui->logWindow->append(QString("<b>txid: </b>") + QString(txid));

        for (int i = 0; i<5; i++) {
            //ui->MainWindow->statusBar()->showMessage("Bet in progress ["+QString::number(5-i)+"]");
            ui->statusBar->showMessage("Bet in progress ["+QString::number(5-i)+"]");
            //sleep(1);
            BetThread::msleep(1000);

        }
        ui->statusBar->showMessage("");

        int row = ui->tableHistory->rowCount();
        QTableWidgetItem *item;
        ui->tableHistory->insertRow(row);
        item = new QTableWidgetItem();
         //item->setData(Qt::DisplayRole,QString(QDateTime::fromTime_t(time(NULL)).toString("dd/MM/yyyy hh:mm:ss")));
        item->setData(Qt::DisplayRole,QString(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss")));
        ui->tableHistory->setItem(row, 0, item);
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole,QString(txid));
        ui->tableHistory->setItem(row, 1, item);
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole,QString::number(amount, 'f', 8));
        ui->tableHistory->setItem(row, 2, item);
        item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole,QString::number(odds));
        ui->tableHistory->setItem(row, 3, item);
        item = new QTableWidgetItem();

        bet_result = dicestatus(txid, &won, res.name, res.fundingtxid);

        switch (bet_result) {
        case 1:
            ui->logWindow->append(QString("<font color=\"green\"><b>WIN</b></font> - ") + QString::number(won, 'f', 8));
            item->setData(Qt::DisplayRole,QString("WIN"));
            break;
        case 0:
            ui->logWindow->append(QString("<font color=\"red\"><b>LOSS</b></font>"));
            item->setData(Qt::DisplayRole,QString("LOSS"));
            break;
        case 2:
            ui->logWindow->append(QString("<b>PENDING</b>"));
            item->setData(Qt::DisplayRole,QString("PENDING"));
            break;
        default:
            ui->logWindow->append(QString("<b>ERROR</b>"));
            item->setData(Qt::DisplayRole,QString("ERROR"));
            break;
        }

        ui->tableHistory->setItem(row, 4, item);

        // ui->tableHistory->resizeColumnsToContents();

        //ui->betButton->setEnabled(true);
        //QCoreApplication::processEvents();
    }


}
