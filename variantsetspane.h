#ifndef VARIANTSETSPANE_H
#define VARIANTSETSPANE_H

#include <QWidget>
#include <QString>
#include "widgetitem.h"
#include "listwidget.h"
#include "deltaxbinder.h"   // variant server to binder
#include "commonutils.h"

class VariantSetsPane : public QWidget
{
    Q_OBJECT

public:
    explicit VariantSetsPane(QWidget* parent = 0);
    virtual ~VariantSetsPane();

    void SetListWidgetSpace(const int space) { m_itemSpace = space; }
    void ClearWidgets();

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    void init();
    void final();

private:
    QWidget*        m_pMainPane;
    ListWidget*     m_pListWidget;
    ListWidget*     m_pItemListWidget;

    int             m_menuIndex;
    int             m_itemSpace;

signals:
    void VariantSetUpdate(const QString&);

public slots:
    void OnVariantSetsListUpdate();

private slots:
    void OnMenuListSelectChanged(int);
    void OnItemListSelectChanged(int);
};

#endif // VARIANTSETSPANE_H
