#ifndef ADDNOTEFOLDERDIALOG_H
#define ADDNOTEFOLDERDIALOG_H

#include <QDialog>
#include "ui_AddNoteFolderDialog.h"

class AddNoteFolderDialog : public QDialog
{
	Q_OBJECT

public:
	AddNoteFolderDialog(QWidget *parent = 0);
	~AddNoteFolderDialog();

	QString GetNewFolderName();

protected slots:
	void OnOkButtonClicked();
	void OnCancelButtonClicked();

private:
	Ui::AddNoteFolderDialog ui;
	QString m_newFolderName;
};

#endif // ADDNOTEFOLDERDIALOG_H
