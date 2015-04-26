#include "AddNoteFolderDialog.h"
#include "DatabaseService.h"
#include <QMessageBox>

AddNoteFolderDialog::AddNoteFolderDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.m_okButton, &QPushButton::clicked, this, &AddNoteFolderDialog::OnOkButtonClicked);
	connect(ui.m_cancelButton, &QPushButton::clicked, this, &AddNoteFolderDialog::OnCancelButtonClicked);
}

AddNoteFolderDialog::~AddNoteFolderDialog()
{

}

void AddNoteFolderDialog::OnOkButtonClicked()
{
	QString newFolderName = ui.m_newFolderNameEdit->text();
	if (newFolderName.isEmpty())
	{
		QMessageBox::information(this, tr("prompt"), tr("The folder name should not be empty."));
		return;
	}

	if (DatabaseService::Instance().FolderExist(newFolderName))
	{
		QMessageBox::information(this, tr("prompt"), tr("The folder name already exists."));
		return;
	}

	m_newFolderName = newFolderName;
	accept();
}

void AddNoteFolderDialog::OnCancelButtonClicked()
{
	reject();
}

QString AddNoteFolderDialog::GetNewFolderName()
{
	return m_newFolderName;
}
