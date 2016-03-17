#ifndef JOBLISTDIAG_H
#define JOBLISTDIAG_H

#include <QWidget>
#include <QList>

#include "jobtaskitem.h"
#include "uianimationmanager.h"

class QString;
class QScrollArea;
class QEvent;

#define BORDER_SPACE        (1)

namespace Ui {
class JobListDiag;
}

class JobListDiag : public QWidget
{
    Q_OBJECT

    enum AnimationType{AnimationType_Invalid = -1, AnimationType_Stretch, AnimationType_Pinch};

public:
    explicit JobListDiag(QWidget *parent = 0);
    ~JobListDiag();

    virtual bool eventFilter(QObject *, QEvent *);

    void addItem(const QString& titleName, const QString& size, const QString& quality);
    void ClearList();

private:
    void init();
    void final();

private slots:
    void OnAnimationFinished();

signals:
    void ButtonClicked();

private:
    Ui::JobListDiag *ui;

    QScrollArea*            m_pScrollArea;
    QWidget                 m_itemsPane;
    QList<JobTaskItem*>     m_jobItems;

    UIAnimationManager      m_animationManager;
    AnimationType           m_animationType;

};

#endif // JOBLISTDIAG_H
