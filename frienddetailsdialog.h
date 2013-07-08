#ifndef FRIENDDETAILSDIALOG_H
#define FRIENDDETAILSDIALOG_H

#include <QDialog>
#include <string>

namespace Ui {
    class FriendDetailsDialog;
}

class FriendDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FriendDetailsDialog(QWidget *parent = 0, const std::string & uid = "Error. No id selected!" );
    ~FriendDetailsDialog();

private:
    Ui::FriendDetailsDialog *ui;
};

#endif // FRIENDDETAILSDIALOG_H
