#ifndef JOBTASKITEM_H
#define JOBTASKITEM_H

#include <QWidget>

namespace Ui {
class JobTaskItem;
}

#define JOB_ITEM_HEAD_HEIGHT    (60)
#define JOB_ITEM_TOTAL_HEIGHT   (130)
#define JOB_ITEM_BODY_HEIGHT    (JOB_ITEM_TOTAL_HEIGHT-JOB_ITEM_HEAD_HEIGHT)

class JobTaskItem : public QWidget
{
    Q_OBJECT

public:
    explicit JobTaskItem(QWidget *parent = 0);
    ~JobTaskItem();

    void SetTitle(const QString&);
    void SetSizeValue(const QString&);
    void SetQualityValue(const QString&);
    void SetBodyVisible(bool);
    bool BodyVisible();

private:
    Ui::JobTaskItem *ui;
};

#endif // JOBTASKITEM_H
