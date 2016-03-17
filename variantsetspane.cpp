#include "variantsetspane.h"
#include "commonutils.h"
#include "resourcemanager.h"
#include <QResizeEvent>

VariantSetsPane::VariantSetsPane(QWidget *parent)
: QWidget(parent)
, m_pMainPane(NULL)
, m_pListWidget(NULL)
, m_pItemListWidget(NULL)
, m_menuIndex(-1)
, m_itemSpace(0)
{
    m_pMainPane = new QWidget(this);
    resize(640, 480);

    init();
}

VariantSetsPane::~VariantSetsPane()
{
    final();

    delete m_pMainPane;
    m_pMainPane = NULL;
}

void VariantSetsPane::init()
{
    // widget list init
    m_pListWidget = new ListWidget(m_pMainPane);
    m_pListWidget->setGeometry(m_itemSpace, m_itemSpace, width()/2-m_itemSpace, height());
    m_pListWidget->SetBackGroundColor(QColor(27, 26, 31));

    m_pItemListWidget = new ListWidget(m_pMainPane);
    m_pItemListWidget->setGeometry(width()/2+m_itemSpace, m_itemSpace, width()/2-m_itemSpace, height());
    m_pItemListWidget->SetBackGroundColor(QColor(51, 50, 56));

    connect(m_pListWidget, SIGNAL(SelecteChanged(int)), this, SLOT(OnMenuListSelectChanged(int)));
    connect(m_pItemListWidget, SIGNAL(SelecteChanged(int)), this, SLOT(OnItemListSelectChanged(int)));
}

void VariantSetsPane::final()
{
    disconnect(m_pListWidget, SIGNAL(SelecteChanged(int)), this, SLOT(OnMenuListSelectChanged(int)));
    disconnect(m_pItemListWidget, SIGNAL(SelecteChanged(int)), this, SLOT(OnItemListSelectChanged(int)));
}

void VariantSetsPane::resizeEvent(QResizeEvent *e)
{
    QSize newSize = e->size();
    resize(newSize);

    m_pMainPane->resize(newSize);
    m_pListWidget->setGeometry(m_itemSpace, m_itemSpace, width()/2-m_itemSpace, height());
    m_pItemListWidget->setGeometry(width()/2+m_itemSpace, m_itemSpace, width()/2-m_itemSpace, height());
}

void VariantSetsPane::OnVariantSetsListUpdate()
{
    QList<VarTree> list = ResourceManagerInst.GetVariantSets();
    QList<WidgetItem*> widgetItemList;

    int index = 0;
    foreach (VarTree var, list) {
        WidgetItem* pItem = new WidgetItem();
  //      qDebug() << QString::number(var.type, 16) << QString(9) << QString(9) << var.varTreeName;
        CommonUtils::IconId id = CommonUtils::GetIconId(var.type);
        if(id < CommonUtils::IconId_Count)  pItem->SetIcon((ResourceManagerInst.GetMenuIcons())[id]);
        pItem->SetText(var.varTreeName);
        pItem->SetBackgroundColor((index%2==0)?QColor(44, 43, 48):QColor(38, 37, 42));
        widgetItemList.append(pItem);

        index++;
    }
    m_pItemListWidget->Clear();
    m_pListWidget->Clear();
    m_pListWidget->AddWidgetItems(widgetItemList);
}

void VariantSetsPane::OnMenuListSelectChanged(int index)
{
    QList<VarTree> list = ResourceManagerInst.GetVariantSets();
    QList<QString> varValueList = list[index].varTreeValue;
    QString curSelectedItem = list[index].currentVarValue;
 //   bool visible = list[index].visible;
    unsigned long type = list[index].type;
    QList<WidgetItem*> widgetItemList;

    int i = 0;
    for(i = 0; i < varValueList.count(); i++) {
        WidgetItem* pItem = new WidgetItem();
        CommonUtils::IconId id = CommonUtils::GetIconId(type);
        if(id < CommonUtils::IconId_Count)  pItem->SetIcon((ResourceManagerInst.GetItemIcons())[id]);
        pItem->SetText(varValueList[i]);
        pItem->SetBackgroundColor((i%2==0)?QColor(44, 43, 48):QColor(38, 37, 42));
        widgetItemList.append(pItem);
    }

    m_pItemListWidget->Clear();
    m_pItemListWidget->AddWidgetItems(widgetItemList);
    m_pItemListWidget->SetCursorIndex(varValueList.indexOf(curSelectedItem));

    m_menuIndex = index;
}

void VariantSetsPane::OnItemListSelectChanged(int index)
{
    QList<VarTree> list = ResourceManagerInst.GetVariantSets();
    if( (m_menuIndex >= list.count()) || (m_menuIndex < 0) ) return;
    VarTree varTree = list[m_menuIndex];
    QString varName = varTree.varTreeName;
    if( (index >= varTree.varTreeValue.count()) || (index < 0) ) return;
    QString varValue = varTree.varTreeValue[index];
    varTree.currentVarValue = varValue;

    ResourceManagerInst.UpdateVariantSets(varTree ,m_menuIndex);

    QString msg = QString("VariantSet(%1:%2);").arg(varName).arg(varValue);

    emit VariantSetUpdate(msg);
}

void VariantSetsPane::ClearWidgets()
{
    m_pListWidget->Clear();
    m_pItemListWidget->Clear();
}
