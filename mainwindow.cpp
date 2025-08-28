#include "mainwindow.h"
#include "./ui_mainwindow.h"

#ifndef active_cell_flag
#define active_cell_flag default_cell_flag | Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsEditable
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->slicer1->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->slicer2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->slicer1->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->slicer2->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    default_cell_flag = ui->slicer1->item(0,0)->flags();
    counter = 0;
    inreset = false;
    connect(this,&MainWindow::i_finished_my_job,this,[&](qint32 ret_value) {
        ui->logger->appendPlainText(QString("%2. Program exited with code %1").arg(ret_value).arg(ui->sub_prc_count->value() - counter + 1));
        if(ret_value == 0){
            if(--counter == 0){
                concat_files();
                if(ui->obf_mode_button->isChecked())
                    QMessageBox::information(this,"Success","Obfuscation completed succesfully.\nPlease take a note tables and secret keys for de-obfuscation.");
                else if(ui->dobf_mode_button->isChecked())
                    QMessageBox::information(this,"Success","De-obfuscation completed succesfully.");
                reset(true);
            }
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void thr1(const QString& program,const QStringList& commandline, MainWindow* window){
    auto response = QProcess::execute(program,commandline);
    emit window->i_finished_my_job(response);
}

void MainWindow::concat_files(){
    QString files_to_concatenate;
    auto out_file = ui->dst_loc->text();
    for (int var = 0; var < ui->sub_prc_count->value(); ++var) {
        files_to_concatenate.append("\"" + out_file).append(".part").append(QString("%1\" ").arg(var + 1));
    }
#ifdef __linux__
    system(("cat " + files_to_concatenate + "> \"" + out_file + "\"").toStdString().c_str());
#elif _WIN32
    system(("type " + files_to_concatenate + "> \"" + out_file + "\"").toStdString().c_str());
#endif
}

void MainWindow::on_src_selector_clicked()
{
    file_src = QFileDialog::getOpenFileName(this,"Source File Chooser");
    ui->src_loc->setText(file_src);
}


void MainWindow::on_dst_selector_clicked()
{
    file_dst = QFileDialog::getSaveFileName(this,"Destination File Chooser");
    ui->dst_loc->setText(file_dst);
}

void MainWindow::src_or_dst_changed(const QString &text){
    if(QFile::exists(ui->src_loc->text()) && !ui->dst_loc->text().isEmpty()){
        ui->seed1->setEnabled(true);
        ui->seed2->setEnabled(true);
        secret_checker();
    }
    else if(ui->dst_loc->text().isEmpty() && QFile::exists(ui->src_loc->text())){
        ui->label_6->setText("<html><head/><body><p><span style=\" font-size:12pt; color:#f20004;\">Destination file location input cannot be empty.</span></p></body></html>");
        ui->kontrol->setCheckState(Qt::Unchecked);
        reset(false);
        reset_tables();
    }
    else{
        ui->label_6->setText("<html><head/><body><p><span style=\" font-size:12pt; color:#f20004;\">Source file doesn't exists.</span></p></body></html>");
        ui->kontrol->setCheckState(Qt::Unchecked);
        reset(false);
        reset_tables();
    }
}

void MainWindow::secret_checker(){

    // *** CHECK SEED1 AND SEED2 *** //

    if(ui->seed1->text() == "" || ui->seed2->text() == ""){
        ui->label_6->setText("<html><head/><body><p><span style=\" font-size:12pt; color:#f20004;\">Secret 1 and Secret 2 musn't be blank.</span></p></body></html>");
        ui->sub_prc_count->setEnabled(false);
        ui->slicer1->setEnabled(false);
        ui->slicer2->setEnabled(false);
        ui->starter->setEnabled(false);
        ui->stopper->setEnabled(false);
        ui->obf_mode_button->setEnabled(false);
        ui->dobf_mode_button->setEnabled(false);
        reset_tables();
    }
    else{

        // CHECKING SEED1 AND SEED2 ARE NUMERIC

        bool ok;
        ui->seed1->text().toULongLong(&ok);
        ui->seed2->text().toULongLong(ok ? &ok : nullptr);
        if(!ok){
            ui->label_6->setText("<html><head/><body><p><span style=\" font-size:12pt; color:#f20004;\">Secret 1 and Secret 2 must be numerical.</span></p></body></html>");
            ui->sub_prc_count->setEnabled(false);
            ui->slicer1->setEnabled(false);
            ui->slicer2->setEnabled(false);
            ui->starter->setEnabled(false);
            ui->stopper->setEnabled(false);
            ui->obf_mode_button->setEnabled(false);
            ui->dobf_mode_button->setEnabled(false);
            reset_tables();
            return;
        }

        ui->label_6->setText(nullptr);
        ui->sub_prc_count->setEnabled(true);
        ui->slicer1->setEnabled(true);
        ui->slicer2->setEnabled(true);
        ui->starter->setEnabled(true);
        ui->stopper->setEnabled(false);
        ui->obf_mode_button->setEnabled(true);
        ui->dobf_mode_button->setEnabled(true);
        ui->sub_prc_count->setValue(1);
        on_sub_prc_count_valueChanged(1);
    }

    // ***                       *** //
}

void MainWindow::on_sub_prc_count_valueChanged(int arg1)
{

    // INITIALIZE TABLE

    reset_tables();
    ui->slicer1->item(0,0)->setText("0");
    ui->slicer2->item(0,0)->setText("0");
    counter = arg1;
    QFile src_file(ui->src_loc->text());
    if(arg1 > 1){
        ui->slicer1->item(0,1)->setFlags(active_cell_flag);
        ui->slicer2->item(0,1)->setFlags(active_cell_flag);
        if(ui->obf_mode_button->isChecked()){
            ui->slicer1->item(arg1 - 1,1)->setText(QString("%1").arg(src_file.size() - 1));
            ui->slicer1->item(arg1 - 1,1)->setFlags(default_cell_flag);
        }
        else if(ui->dobf_mode_button->isChecked()){
            ui->slicer2->item(arg1 - 1,1)->setText(QString("%1").arg(src_file.size() - 1));
            ui->slicer2->item(arg1 - 1,1)->setFlags(default_cell_flag);
        }
    }
    else if (arg1 == 1){
        ui->slicer1->item(0,1)->setFlags(default_cell_flag);
        ui->slicer2->item(0,1)->setFlags(default_cell_flag);
        if(ui->obf_mode_button->isChecked()){
            ui->slicer1->item(0,1)->setText(QString("%1").arg(src_file.size() - 1));
            ui->slicer2->item(0,1)->setFlags(active_cell_flag);
        }

        else if(ui->dobf_mode_button->isChecked()){
            ui->slicer2->item(0,1)->setText(QString("%1").arg(src_file.size() - 1));
            ui->slicer1->item(0,1)->setFlags(active_cell_flag);
        }
    }
}

void MainWindow::reset_tables(){
    inreset = true;
    for (size_t i = 0; i < ui->slicer1->rowCount(); ++i) {
        for (size_t j = 0; j < ui->slicer1->columnCount(); ++j) {
            auto item = ui->slicer1->item(i,j);
            item->setFlags(default_cell_flag);
            item->setText(nullptr);
        }
    }
    for (size_t i = 0; i < ui->slicer2->rowCount(); ++i) {
        for (size_t j = 0; j < ui->slicer2->columnCount(); ++j) {
            auto item = ui->slicer2->item(i,j);
            item->setFlags(default_cell_flag);
            item->setText(nullptr);
        }
    }
    inreset = false;
}

void MainWindow::update_table(int row, int column, QTableWidget* table){
    if(inreset | (column == 0))
        return;
    auto current_value = table->item(row,column)->text().toULongLong();

    // *** CHECK TABLES *** //

    ui->kontrol->setChecked(true);
    //ui->label_6->setText(nullptr);
    for (size_t var = 0; var < counter; ++var) {
        if(ui->slicer1->item(var,1)->text().isEmpty() || ui->slicer1->item(var,0)->text().toULongLong() > ui->slicer1->item(var,1)->text().toULongLong()){
            ui->label_6->setText(QString("<html><head/><body><p><span style=\" font-size:12pt; color:#f20004;\">table1(%1,%2) cannot be greater than table1(%1,%3)</span></p></body></html>").arg(var + 1)
                                     .arg(1).arg(2));
            ui->kontrol->setChecked(false);
            break;
        }
        if(ui->slicer2->item(var,0)->text().toULongLong() > ui->slicer2->item(var,1)->text().toULongLong()){
            ui->label_6->setText(QString("<html><head/><body><p><span style=\" font-size:12pt; color:#f20004;\">table2(%1,%2) cannot be greater than table2(%1,%3)</span></p></body></html>").arg(var + 1)
                                     .arg(1).arg(2));
            ui->kontrol->setChecked(false);
            break;
        }
        if(ui->slicer1->item(var,1)->text().toULongLong() - ui->slicer1->item(var,0)->text().toULongLong() > ui->slicer2->item(var,1)->text().toULongLong() - ui->slicer2->item(var,0)->text().toULongLong()){
            ui->label_6->setText(QString("<html><head/><body><p><span style=\" font-size:12pt; color:#f20004;\">table1(%1,%2) - table1(%1,%3) cannot be greater than table2(%1,%2) - table2(%1,%3)</span></p></body></html>").arg(var + 1).arg(2).arg(1));
            ui->kontrol->setChecked(false);
            break;
        }
    }

    // ***              *** //

    if(row < ui->sub_prc_count->value() - 1){
        if(table->item(row,column - 1)->text().toULongLong() < current_value){
            table->item(row + 1, column - 1)->setText(QString("%1").arg(current_value + 1));
            if(table->item(row + 1, column)->text().isEmpty())
                table->item(row + 1, column)->setFlags(active_cell_flag);
        }
    }
}

void MainWindow::reset(bool value){
    if(value){
        ui->src_loc->setEnabled(true);
        ui->dst_loc->setEnabled(true);
        ui->src_selector->setEnabled(true);
        ui->dst_selector->setEnabled(true);
        ui->stopper->setEnabled(false);
        counter = ui->sub_prc_count->value(); // PROBLEMATIC
    }
    ui->seed1->setEnabled(value);
    ui->seed2->setEnabled(value);
    ui->sub_prc_count->setEnabled(value);
    ui->slicer1->setEnabled(value);
    ui->slicer2->setEnabled(value);
    ui->starter->setEnabled(value);
    ui->obf_mode_button->setEnabled(value);
    ui->dobf_mode_button->setEnabled(value);
}

void MainWindow::on_obf_mode_button_toggled(bool checked)
{
    reset_tables();
    ui->sub_prc_count->setValue(1);
    on_sub_prc_count_valueChanged(1);
}


void MainWindow::on_dobf_mode_button_toggled(bool checked)
{
    reset_tables();
    ui->sub_prc_count->setValue(1);
    on_sub_prc_count_valueChanged(1);
}


void MainWindow::on_slicer1_cellChanged(int row, int column)
{
    update_table(row,column,ui->slicer1);
}


void MainWindow::on_slicer2_cellChanged(int row, int column)
{
    update_table(row,column,ui->slicer2);
}


void MainWindow::on_starter_clicked()
{
    // PASSING ARGUMENTS TO SUBPROCESSES

    if(ui->kontrol->isChecked()){
        ui->src_loc->setEnabled(false);
        ui->dst_loc->setEnabled(false);
        ui->src_selector->setEnabled(false);
        ui->dst_selector->setEnabled(false);
        reset(false);
        ui->stopper->setEnabled(true);
        ui->logger->clear();
        QStringList arguments{9};
        if(ui->obf_mode_button->isChecked()){
            auto obf_loc = QFileDialog::getOpenFileName(this,"Choose obfuscator location");
            for (size_t var = 0; var < counter; ++var) {
                arguments.assign({ui->seed1->text().trimmed(),ui->seed2->text().trimmed(),
                                  ui->slicer1->item(var,0)->text().trimmed(),ui->slicer1->item(var,1)->text().trimmed(),
                                  ui->slicer2->item(var,0)->text().trimmed(),ui->slicer2->item(var,1)->text().trimmed(),
                                  ui->src_loc->text().trimmed(),ui->dst_loc->text().trimmed(),QString("%1").arg(var+1)});
                QtConcurrent::run(thr1,obf_loc,arguments,this);
            }
        }
        else if(ui->dobf_mode_button->isChecked()){
            auto dobf_loc = QFileDialog::getOpenFileName(this,"Choose de-obfuscator location");
            for (size_t var = 0; var < counter; ++var) {
                arguments.assign({ui->seed1->text().trimmed(),ui->seed2->text().trimmed(),
                                  ui->slicer2->item(var,0)->text().trimmed(),ui->slicer2->item(var,1)->text().trimmed(),
                                  ui->slicer1->item(var,0)->text().trimmed(),ui->slicer1->item(var,1)->text().trimmed(),
                                  ui->src_loc->text().trimmed(),ui->dst_loc->text().trimmed(),QString("%1").arg(var+1)});
                QtConcurrent::run(thr1,dobf_loc,arguments,this);
            }
        }
    }
}


void MainWindow::on_stopper_clicked()
{
// ----- //
}

