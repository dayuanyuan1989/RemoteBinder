#ifndef PANE_BASE_H
#define PANE_BASE_H

#include <QWidget>

class PaneBase : public QWidget
{
	Q_OBJECT

public:
    PaneBase(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	virtual ~PaneBase();
};

#endif	//PANE_BASE_H
