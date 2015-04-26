#ifndef NOTEITEMWIDGET_H
#define NOTEITEMWIDGET_H

#include <QWidget>
#include "ui_NoteItemWidget.h"
#include "defines.h"

class NoteItemWidget : public QWidget
{
	Q_OBJECT

public:
	NoteItemWidget(QWidget *parent = 0);
	~NoteItemWidget();

public:
	void UpdateFromNote(const Note& noteData);
	Note GetNote();

private:
	Ui::NoteItemWidget ui;
	Note m_noteData;
};

#endif // NOTEITEMWIDGET_H
