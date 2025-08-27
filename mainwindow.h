#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QMessageBox>
#include <QtCore/QProcess>
#include <QtCore/QtTypes>
#include <QtConcurrent/QtConcurrentRun>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void i_finished_my_job(qint32 ret_value);

private slots:
    void on_src_selector_clicked();

    void on_dst_selector_clicked();

    void src_or_dst_changed(const QString &text);

    void on_sub_prc_count_valueChanged(int arg1);

    void on_obf_mode_button_toggled(bool checked);

    void on_dobf_mode_button_toggled(bool checked);

    void on_slicer1_cellChanged(int row, int column);

    void on_slicer2_cellChanged(int row, int column);

    void on_starter_clicked();

    void on_stopper_clicked();

    void secret_checker();

private:
    void reset(bool value);
    void reset_tables();
    void update_table(int row, int column, QTableWidget* item);
    void concat_files();

    bool inreset;
    qint32 counter;
    Ui::MainWindow *ui;
    QString file_src,file_dst;
    Qt::ItemFlags default_cell_flag;
};

#endif // MAINWINDOW_H
