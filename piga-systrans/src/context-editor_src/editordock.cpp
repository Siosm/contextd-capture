#include "editordock.h"

EditorDock::EditorDock(QWidget *parent) :
	QDockWidget(parent)
{
}

EditorDock::~EditorDock()
{
}

bool EditorDock::isValid() const
{
	return false;
}

void EditorDock::closeEvent(QCloseEvent *e)
{
	emit dockClosed();
	e->accept();
}

void EditorDock::updateDock()
{
}

void EditorDock::resetDock()
{
}
