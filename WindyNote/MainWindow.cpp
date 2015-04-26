#include "MainWindow.h"
#include <QWebPage>
#include <QWebFrame>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QMessageBox>
#include <QCursor>
#include <QDebug>
#include <QScreen>
#include "NoteItemWidget.h"
#include "DatabaseService.h"
#include "AddNoteFolderDialog.h"
#include "DragProxy.h"

const int NOTE_FOLDER_ID_ROLE = Qt::UserRole + 1;
const int NOTE_ITEM_HEIGHT = 80;

MainWindow::MainWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	DragProxy* dragProxy = new DragProxy(this);
	dragProxy->SetBorderWidth(3, 3, 3, 3);

	InitContextMenus();

	ui.m_noteFolderTree->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.m_noteList->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui.m_minButton, &QPushButton::clicked, this, &MainWindow::OnMinButtonClicked);
	connect(ui.m_maxButton, &QPushButton::clicked, this, &MainWindow::OnMaxButtonClicked);
	connect(ui.m_restoreButton, &QPushButton::clicked, this, &MainWindow::OnRestoreButtonClicked);
	connect(ui.m_closeButton, &QPushButton::clicked, this, &MainWindow::OnCloseButtonClicked);

	ui.m_restoreButton->hide();

	connect(ui.m_createNoteButton, &QPushButton::clicked, this, &MainWindow::OnCreateNoteButtonClicked);
	connect(ui.m_noteFolderTree, &QTreeWidget::currentItemChanged, 
		this, &MainWindow::OnNoteFolderItemChanged);
	connect(ui.m_noteList, &QListWidget::currentItemChanged,
		this, &MainWindow::OnNoteItemChanged);

	connect(ui.m_noteFolderTree, &QTreeWidget::customContextMenuRequested,
		this, &MainWindow::OnFolderTreeContext);
	connect(ui.m_noteList, &QListWidget::customContextMenuRequested,
		this, &MainWindow::OnNoteListContext);

	QWebPage* page = ui.m_noteContent->page();
	QWebFrame* mainFrame = page->mainFrame();
	connect(mainFrame, &QWebFrame::loadFinished, this, &MainWindow::OnNoteContentViewLoaded);

	QString localEditor = QCoreApplication::applicationDirPath() + "/ckeditor/editor.html";
	ui.m_noteContent->load(QUrl::fromLocalFile(localEditor));

	LoadNoteFolders();
}

MainWindow::~MainWindow()
{

}

void MainWindow::InitContextMenus()
{
	m_noteFolderTreeContextMenu = new QMenu(this);
	m_addNoteFolderAction = m_noteFolderTreeContextMenu->addAction(tr("add note folder"));
	m_removeNoteFolderAction = m_noteFolderTreeContextMenu->addAction(tr("remove note folder"));

	m_noteListContextMenu = new QMenu(this);
	m_addNoteAction = m_noteListContextMenu->addAction(tr("add note"));
	m_removeNoteAction = m_noteListContextMenu->addAction(tr("remove note"));

	connect(m_addNoteFolderAction, &QAction::triggered,
		this, &MainWindow::OnAddNoteFolderActionTriggered);
	connect(m_removeNoteFolderAction, &QAction::triggered,
		this, &MainWindow::OnRemoveNoteFolderActionTriggered);
	connect(m_addNoteAction, &QAction::triggered,
		this, &MainWindow::OnAddNoteActionTriggered);
	connect(m_removeNoteAction, &QAction::triggered,
		this, &MainWindow::OnRemoveNoteActionTriggered);
}

void MainWindow::LoadNoteFolders()
{
	QVector<NoteFolder> noteFolders = DatabaseService::Instance().GetNoteFolders();
	for (int i = 0; i < noteFolders.size(); i++)
	{
		const NoteFolder& noteFolderData = noteFolders[i];
		QTreeWidgetItem* folderItem = new QTreeWidgetItem(QStringList() << noteFolderData.folderName);
		folderItem->setData(0, NOTE_FOLDER_ID_ROLE, noteFolderData.folderId);
		ui.m_noteFolderTree->addTopLevelItem(folderItem);
	}
}

void MainWindow::OnNoteContentViewLoaded(bool ok)
{

}

void MainWindow::OnCreateNoteButtonClicked()
{
	QTreeWidgetItem* curItem = ui.m_noteFolderTree->currentItem();
	if (!curItem)
	{
		return;
	}

	int curNoteFolderId = curItem->data(0, NOTE_FOLDER_ID_ROLE).toInt();
	AddNote("new note", "", curNoteFolderId);
}

QString MainWindow::GetEditorContent()
{
	return ui.m_noteContent->page()->mainFrame()->evaluateJavaScript("getEditorContent()").toString();
}

void MainWindow::SetEditorContent(const QString& content)
{
	QString processedContent = content;
	processedContent.replace("\n", "");
	ui.m_noteContent->page()->mainFrame()->evaluateJavaScript("setEditorContent('" + processedContent + "')");
}

void MainWindow::OnNoteFolderItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	ui.m_noteList->clear();

	int curNoteFolderId = current->data(0, NOTE_FOLDER_ID_ROLE).toInt();
	QVector<Note> notes = DatabaseService::Instance().GetNotes(curNoteFolderId);
	for (int i = 0; i < notes.size(); i++)
	{
		const Note& noteData = notes[i];
		QListWidgetItem* item = new QListWidgetItem(ui.m_noteList);
		item->setSizeHint(QSize(100, NOTE_ITEM_HEIGHT));
		NoteItemWidget* itemWidget = new NoteItemWidget(ui.m_noteList);
		itemWidget->UpdateFromNote(noteData);
		ui.m_noteList->setItemWidget(item, itemWidget);
	}

}

void MainWindow::OnNoteItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (previous)
	{
		NoteItemWidget* preItemWidget = static_cast<NoteItemWidget*>(ui.m_noteList->itemWidget(previous));
		if (preItemWidget)
		{
			Note oldNoteData = preItemWidget->GetNote();
			UpdateNote(oldNoteData.noteId);
			Note newNoteData = DatabaseService::Instance().GetNoteById(oldNoteData.noteId);
			preItemWidget->UpdateFromNote(newNoteData);
		}
	}

	if (!current)
	{
		return;
	}

	NoteItemWidget* curItemWidget = static_cast<NoteItemWidget*>(ui.m_noteList->itemWidget(current));
	if (curItemWidget)
	{
		Note noteData = curItemWidget->GetNote();
		ui.m_subjectEdit->setText(noteData.subject);
		SetEditorContent(noteData.content);
	}
}

void MainWindow::OnFolderTreeContext(const QPoint& pos)
{
	QTreeWidgetItem* item = ui.m_noteFolderTree->itemAt(pos);
	if (!item)
		return;

	m_noteFolderTreeContextMenu->popup(QCursor::pos());
}

void MainWindow::OnNoteListContext(const QPoint& pos)
{
	QListWidgetItem* item = ui.m_noteList->itemAt(pos);
	if (!item)
		return;

	m_noteListContextMenu->popup(QCursor::pos());
}

void MainWindow::OnAddNoteFolderActionTriggered()
{
	AddNoteFolder();
}

void MainWindow::OnRemoveNoteFolderActionTriggered()
{
	QTreeWidgetItem* curItem = ui.m_noteFolderTree->currentItem();
	if (!curItem)
	{
		return;
	}

	int curNoteFolderId = curItem->data(0, NOTE_FOLDER_ID_ROLE).toInt();
	RemoveNoteFolderById(curNoteFolderId);
}

void MainWindow::OnAddNoteActionTriggered()
{
	OnCreateNoteButtonClicked();
}

void MainWindow::OnRemoveNoteActionTriggered()
{
	QListWidgetItem* curItem = ui.m_noteList->currentItem();
	if (!curItem)
		return;

	NoteItemWidget* itemWidget = static_cast<NoteItemWidget*>(ui.m_noteList->itemWidget(curItem));
	DatabaseService::Instance().RemoveNote(itemWidget->GetNote().noteId);
	int index = ui.m_noteList->row(curItem);
	ui.m_noteList->takeItem(index);
}

void MainWindow::AddNoteFolder()
{
	AddNoteFolderDialog dlg(this);
	if (dlg.exec() == QDialog::Accepted)
	{
		QString newFolderName = dlg.GetNewFolderName();
		int newFolderId = 0;
		if (DatabaseService::Instance().CreateNoteFolder(newFolderName, -1, &newFolderId))
		{
			QTreeWidgetItem* folderItem = new QTreeWidgetItem(QStringList() << newFolderName);
			folderItem->setData(0, NOTE_FOLDER_ID_ROLE, newFolderId);
			ui.m_noteFolderTree->addTopLevelItem(folderItem);
		}
	}
}

void MainWindow::RemoveNoteFolderById(int noteFolderId)
{
	DatabaseService::Instance().RemoveNoteFolder(noteFolderId);

	int noteFolderItemCount = ui.m_noteFolderTree->topLevelItemCount();
	for (int i = 0; i < noteFolderItemCount; i++)
	{
		QTreeWidgetItem* item = ui.m_noteFolderTree->topLevelItem(i);
		int id = item->data(0, NOTE_FOLDER_ID_ROLE).toInt();
		if (id == noteFolderId)
		{
			ui.m_noteFolderTree->takeTopLevelItem(i);
			return;
		}
	}
}

void MainWindow::AddNote(const QString& subject, const QString& content, int noteFolderId)
{
	int newNoteId = 0;
	DatabaseService::Instance().CreateNote(subject, content, noteFolderId, &newNoteId);

	QTreeWidgetItem* curItem = ui.m_noteFolderTree->currentItem();
	if (!curItem)
	{
		return;
	}

	int curNoteFolderId = curItem->data(0, NOTE_FOLDER_ID_ROLE).toInt();
	if (curNoteFolderId == noteFolderId)
	{
		Note noteData = DatabaseService::Instance().GetNoteById(newNoteId);
		
		QListWidgetItem* item = new QListWidgetItem;
		item->setSizeHint(QSize(100, NOTE_ITEM_HEIGHT));
		NoteItemWidget* itemWidget = new NoteItemWidget(ui.m_noteList);
		ui.m_noteList->insertItem(0, item);
		itemWidget->UpdateFromNote(noteData);
		ui.m_noteList->setItemWidget(item, itemWidget);
		ui.m_noteList->setCurrentItem(ui.m_noteList->item(0));
	}
}

void MainWindow::UpdateNote(int noteId)
{
	QString subject = ui.m_subjectEdit->text();
	QString content = GetEditorContent();
	DatabaseService::Instance().ModifyNote(noteId, subject, content);
}

void MainWindow::OnMinButtonClicked()
{
	showMinimized();
}

void MainWindow::OnMaxButtonClicked()
{
	ui.m_maxButton->hide();
	ui.m_restoreButton->show();

	m_normalRect = geometry();
	QRect availableRect = QGuiApplication::primaryScreen()->availableGeometry();
	setGeometry(availableRect);
}

void MainWindow::OnRestoreButtonClicked()
{
	ui.m_maxButton->show();
	ui.m_restoreButton->hide();

	setGeometry(m_normalRect);
}

void MainWindow::OnCloseButtonClicked()
{
	close();
}

