#include "NoteItemWidget.h"

NoteItemWidget::NoteItemWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

NoteItemWidget::~NoteItemWidget()
{

}

void NoteItemWidget::UpdateFromNote(const Note& noteData)
{
	m_noteData = noteData;

	ui.m_subjectLabel->setText(noteData.subject);
	QString dateString;
	dateString.sprintf("%d/%d/%d",
		noteData.lastModifiedTime.date().year(),
		noteData.lastModifiedTime.date().month(),
		noteData.lastModifiedTime.date().day());
	ui.m_dateLabel->setText(dateString);
	QString content = noteData.content;
	ui.m_contentLabel->setText(content.replace(QRegExp("<[^>]*>"), "").replace("\n", ""));
}

Note NoteItemWidget::GetNote()
{
	return m_noteData;
}