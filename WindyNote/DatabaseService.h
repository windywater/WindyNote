#pragma once
#include <QObject>
#include <QSqlDatabase>
#include <QVector>
#include "defines.h"

class DatabaseService : public QObject
{
	Q_OBJECT
private:
	DatabaseService();
	~DatabaseService();
	DatabaseService& operator =(const DatabaseService&);

public:
	static DatabaseService& Instance();
	void Init();
	void Uninit();

	bool CreateNoteFolder(const QString& folderName, int parentNoteFolderId, int* newNoteFolderId = nullptr);
	bool RenameNoteFolder(int noteFolderId, const QString& newFolderName);
	bool RemoveNoteFolder(int noteFolderId);

	bool CreateNote(const QString& subject, const QString& content, int noteFolderId, int* newNoteId = nullptr);
	bool ModifyNote(int nodeId, const QString& newSubject, const QString& newContent);
	bool RemoveNote(int nodeId);

	NoteFolder GetNoteFoderById(int noteFolderId);
	Note GetNoteById(int noteId);
	QVector<NoteFolder> GetNoteFolders();
	QVector<Note> GetNotes(int folderId);
	bool FolderExist(const QString& folderName);

protected:
	bool DatabaseFileExists();
	void InitDBTables();
	bool OpenDatabase();

	bool DBTableExist(const QString& tableName);
	
protected:
	QSqlDatabase m_database;

};

