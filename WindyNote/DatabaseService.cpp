#include "DatabaseService.h"
#include <QFile>
#include <QSqlQuery>
#include <QDateTime>
#include <QVariant>

DatabaseService::DatabaseService()
{
}

DatabaseService::~DatabaseService()
{
	Uninit();
}

DatabaseService& DatabaseService::Instance()
{
	static DatabaseService instance;
	return instance;
}

void DatabaseService::Init()
{
	bool open = OpenDatabase();
	if (open)
	{
		InitDBTables();
	}

}

void DatabaseService::Uninit()
{
	m_database.close();
}

bool DatabaseService::DatabaseFileExists()
{
	return QFile::exists("notes.db");
}

bool DatabaseService::OpenDatabase()
{
	m_database = QSqlDatabase::addDatabase("QSQLITE");
	m_database.setDatabaseName("notes.db");
	return m_database.open();
}

void DatabaseService::InitDBTables()
{
	QSqlQuery query("", m_database);

	if (!DBTableExist("folders"))
	{
		query.exec("CREATE TABLE folders (folderName varchar(30), parentFolderId int, createdTime int)");
		CreateNoteFolder(tr("default folder"), -1);
	}
	
	if (!DBTableExist("notes"))
	{
		query.exec("CREATE TABLE notes (subject varchar(50), content text, lastModifiedTime int, folderId int)");
	}
}

bool DatabaseService::CreateNoteFolder(const QString& folderName, int parentNoteFolderId, int* newNoteFolderId /*= nullptr*/)
{
	QSqlQuery query("", m_database);
	query.prepare("INSERT INTO folders VALUES (?, ?, ?)");
	query.bindValue(0, folderName);
	query.bindValue(1, parentNoteFolderId);
	query.bindValue(2, QDateTime::currentDateTime().currentMSecsSinceEpoch());
	query.exec();

	if (newNoteFolderId)
	{
		query.exec("SELECT last_insert_rowid()");
		if (query.next())
		{
			*newNoteFolderId = query.value(0).toInt();
		}
	}

	return true;
}

bool DatabaseService::FolderExist(const QString& folderName)
{
	QSqlQuery query("", m_database);
	query.prepare("SELECT COUNT(*) FROM folders WHERE folderName=?");
	query.bindValue(0, folderName);
	query.exec();

	if (query.next())
	{
		return (query.value(0).toInt() > 0);
	}

	return false;
}

bool DatabaseService::RenameNoteFolder(int noteFolderId, const QString& newFolderName)
{
	QSqlQuery query("", m_database);
	query.prepare("UPDATE folders SET folderName=? WHERE rowid=?");
	query.bindValue(0, newFolderName);
	query.bindValue(1, noteFolderId);
	query.exec();

	return true;
}

bool DatabaseService::RemoveNoteFolder(int noteFolderId)
{
	QSqlQuery query("", m_database);
	query.prepare("DELETE FROM folders WHERE rowid=?");
	query.bindValue(0, noteFolderId);
	query.exec();

	query.prepare("DELETE FROM notes WHERE folderId=?");
	query.bindValue(0, noteFolderId);
	query.exec();

	return true;
}

bool DatabaseService::CreateNote(const QString& subject, const QString& content, int noteFolderId, int* newNoteId /*= nullptr*/)
{
	QSqlQuery query("", m_database);
	query.prepare("INSERT INTO notes VALUES (?, ?, ?, ?)");
	query.bindValue(0, subject);
	query.bindValue(1, content);
	query.bindValue(2, QDateTime::currentDateTime().currentMSecsSinceEpoch());
	query.bindValue(3, noteFolderId);
	query.exec();

	if (newNoteId)
	{
		query.exec("SELECT last_insert_rowid()");
		if (query.next())
		{
			*newNoteId = query.value(0).toInt();
		}
	}

	return true;
}

bool DatabaseService::ModifyNote(int nodeId, const QString& newSubject, const QString& newContent)
{
	QSqlQuery query("", m_database);
	query.prepare("UPDATE notes SET subject=?, content=? WHERE rowid=?");
	query.bindValue(0, newSubject);
	query.bindValue(1, newContent);
	query.bindValue(2, nodeId);
	query.exec();

	return true;
}

bool DatabaseService::RemoveNote(int nodeId)
{
	QSqlQuery query("", m_database);
	query.prepare("DELETE FROM notes WHERE rowid=?");
	query.bindValue(0, nodeId);
	query.exec();

	return true;
}

bool DatabaseService::DBTableExist(const QString& tableName)
{
	QSqlQuery query("", m_database);
	query.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name=?");
	query.bindValue(0, tableName);
	query.exec();
	if (query.next())
	{
		return (query.value(0).toInt() > 0);
	}

	return false;
}

NoteFolder DatabaseService::GetNoteFoderById(int noteFolderId)
{
	NoteFolder noteFolderData;

	QSqlQuery query("", m_database);
	query.prepare("SELECT rowid, folderName, parentFolderId, createdTime FROM folders WHERE rowid=?");
	query.bindValue(0, noteFolderId);
	query.exec();

	if (query.next())
	{
		noteFolderData.folderId = query.value(0).toInt();
		noteFolderData.folderName = query.value(1).toString();
		noteFolderData.parentFolderId = query.value(2).toInt();
		noteFolderData.createdTime = QDateTime::fromMSecsSinceEpoch(query.value(3).toInt());
	}

	return noteFolderData;
}

Note DatabaseService::GetNoteById(int noteId)
{
	Note noteData;

	QSqlQuery query("", m_database);
	query.prepare("SELECT rowid, subject, content, lastModifiedTime FROM notes WHERE rowid=?");
	query.bindValue(0, noteId);
	query.exec();

	if (query.next())
	{
		noteData.noteId = query.value(0).toInt();
		noteData.subject = query.value(1).toString();
		noteData.content = query.value(2).toString();
		noteData.lastModifiedTime = QDateTime::fromMSecsSinceEpoch(query.value(3).toLongLong());
	}

	return noteData;
}

QVector<NoteFolder> DatabaseService::GetNoteFolders()
{
	QVector<NoteFolder> noteFolders;

	QSqlQuery query("", m_database);
	query.exec("SELECT rowid, folderName, parentFolderId, createdTime FROM folders ORDER BY createdTime ASC");
	
	NoteFolder noteFolderData;
	while (query.next())
	{
		noteFolderData.folderId = query.value(0).toInt();
		noteFolderData.folderName = query.value(1).toString();
		noteFolderData.parentFolderId = query.value(2).toInt();
		noteFolderData.createdTime = QDateTime::fromMSecsSinceEpoch(query.value(3).toInt());
		noteFolders << noteFolderData;
	}

	return noteFolders;
}

QVector<Note> DatabaseService::GetNotes(int folderId)
{
	QVector<Note> notes;

	QSqlQuery query("", m_database);
	query.prepare("SELECT rowid, subject, content, lastModifiedTime FROM notes WHERE folderId=? ORDER BY lastModifiedTime DESC");
	query.bindValue(0, folderId);
	query.exec();

	Note noteData;
	noteData.belongedFolderId = folderId;
	while (query.next())
	{
		noteData.noteId = query.value(0).toInt();
		noteData.subject = query.value(1).toString();
		noteData.content = query.value(2).toString();
		noteData.lastModifiedTime = QDateTime::fromMSecsSinceEpoch(query.value(3).toLongLong());
		notes << noteData;
	}

	return notes;
}