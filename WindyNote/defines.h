#pragma once
#include <QString>
#include <QDateTime>

struct NoteFolder 
{
	int folderId;
	QString folderName;
	int parentFolderId;
	QDateTime createdTime;
};

struct Note
{
	int noteId;
	QString subject;
	QString content;
	QDateTime lastModifiedTime;
	int belongedFolderId;
};