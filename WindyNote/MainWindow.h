#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QDialog>
#include "ui_MainWindow.h"
#include <QMenu>
#include <QAction>

class MainWindow : public QDialog
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void InitContextMenus();
	void LoadNoteFolders();
	QString GetEditorContent();
	void SetEditorContent(const QString& content);

	void AddNoteFolder();
	void RemoveNoteFolderById(int noteFolderId);
	void AddNote(const QString& subject, const QString& content, int noteFolderId);
	void UpdateNote(int noteId);

protected slots:
	void OnMinButtonClicked();
	void OnMaxButtonClicked();
	void OnRestoreButtonClicked();
	void OnCloseButtonClicked();

	void OnNoteContentViewLoaded(bool ok);
	void OnCreateNoteButtonClicked();
	void OnNoteFolderItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void OnNoteItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
	void OnFolderTreeContext(const QPoint& pos);
	void OnNoteListContext(const QPoint& pos);

	void OnAddNoteFolderActionTriggered();
	void OnRemoveNoteFolderActionTriggered();
	void OnAddNoteActionTriggered();
	void OnRemoveNoteActionTriggered();

private:
	Ui::MainWindowClass ui;
	QMenu* m_noteFolderTreeContextMenu;
	QAction* m_addNoteFolderAction;
	QAction* m_removeNoteFolderAction;

	QMenu* m_noteListContextMenu;
	QAction* m_addNoteAction;
	QAction* m_removeNoteAction;

	QRect m_normalRect;
};

#endif // MAINWINDOW_H
