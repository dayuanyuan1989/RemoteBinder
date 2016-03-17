#include "joblistdiag.h"
#include "ui_joblistdiag.h"

#include <QScrollArea>
#include <QScrollBar>
#include <QEvent>

JobListDiag::JobListDiag(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JobListDiag),
    m_animationType(AnimationType_Invalid)
{
    ui->setupUi(this);

    init();
}

JobListDiag::~JobListDiag()
{
    final();

    delete ui;
}

void JobListDiag::addItem(const QString& titleName, const QString& size, const QString& quality)
{
    m_itemsPane.setObjectName("itemsPane");
    m_itemsPane.setStyleSheet("#itemsPane{background-color: rgb(118, 118, 118);}");
    m_itemsPane.resize(width(), (m_jobItems.count()+1) * (JOB_ITEM_HEAD_HEIGHT + BORDER_SPACE) + BORDER_SPACE);

    JobTaskItem* pItem = new JobTaskItem(&m_itemsPane);
    pItem->installEventFilter(this);
    pItem->SetTitle(titleName);
    pItem->SetSizeValue(size);
    pItem->SetQualityValue(quality);
    pItem->setGeometry(0, m_jobItems.count() * (JOB_ITEM_HEAD_HEIGHT + BORDER_SPACE) + BORDER_SPACE, width(), JOB_ITEM_HEAD_HEIGHT);
    pItem->raise();
    pItem->show();

    m_jobItems.append(pItem);
}

void JobListDiag::init()
{
    m_animationManager.SetDuration(300);

    m_itemsPane.resize(width(), 0);
    // init scroll area
    m_pScrollArea = new QScrollArea(ui->background);
    m_pScrollArea->setObjectName("scrollArea");
    m_pScrollArea->setStyleSheet("#scrollArea{background-color: rgb(118, 118, 118);}");
    m_pScrollArea->resize(width(), height() - ui->bottomWidget->height());
    m_pScrollArea->setWidget(&m_itemsPane);
    m_pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    connect(&m_animationManager, SIGNAL(finished()), this, SLOT(OnAnimationFinished()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SIGNAL(ButtonClicked()));
}

void JobListDiag::final()
{
    disconnect(&m_animationManager, SIGNAL(finished()), this, SLOT(OnAnimationFinished()));
    disconnect(ui->pushButton, SIGNAL(clicked()), this, SIGNAL(ButtonClicked()));
}

void JobListDiag::OnAnimationFinished()
{
    if(AnimationType_Pinch == m_animationType) {
        m_itemsPane.resize(width(), (m_jobItems.count()) * (JOB_ITEM_HEAD_HEIGHT + BORDER_SPACE) + BORDER_SPACE);
    }

    m_animationType = AnimationType_Invalid;
}


void JobListDiag::ClearList()
{
    foreach(JobTaskItem* pItem, m_jobItems) {
        delete pItem;
    }
    m_jobItems.clear();
    m_itemsPane.resize(width(), (m_jobItems.count()) * (JOB_ITEM_HEAD_HEIGHT + BORDER_SPACE) + BORDER_SPACE);
}

bool JobListDiag::eventFilter(QObject *obj, QEvent *e)
{
    int i, j;
    JobTaskItem* pItem = NULL;
    QRect tartgetRect;
    for(i = 0; i < m_jobItems.count(); i++)
    {
        pItem = m_jobItems[i];
        if(pItem == obj) {
            switch(e->type()) {
            case QEvent::MouseButtonRelease:
            {
                if(pItem->BodyVisible()) {
                // 收缩动画
                    // 开始位置
                    m_animationType = AnimationType_Pinch;
                    j = 0;
                    for(j = 0; j < m_jobItems.count(); j++) {
                        tartgetRect.setTopLeft(QPoint(0, j * (JOB_ITEM_HEAD_HEIGHT + BORDER_SPACE) + BORDER_SPACE));
                        tartgetRect.setWidth(width());
                        tartgetRect.setHeight(JOB_ITEM_HEAD_HEIGHT);
                        m_animationManager.AddAnimation(m_jobItems[j], UIAnimationManager::ActionType_Geometry, m_jobItems[j]->geometry(), tartgetRect);
                    }
                } else {
                    // 伸展动画
                    m_animationType = AnimationType_Stretch;
                    j = 0;
                    for(j = 0; j < i; j++) {
                        tartgetRect.setTopLeft(QPoint(0, j * (JOB_ITEM_HEAD_HEIGHT + BORDER_SPACE) + BORDER_SPACE));
                        tartgetRect.setWidth(width());
                        tartgetRect.setHeight(JOB_ITEM_HEAD_HEIGHT);
                        m_animationManager.AddAnimation(m_jobItems[j], UIAnimationManager::ActionType_Geometry, m_jobItems[j]->geometry() , tartgetRect);
                    }
                    tartgetRect.setTopLeft(QPoint(0, j * (JOB_ITEM_HEAD_HEIGHT + BORDER_SPACE) + BORDER_SPACE));
                    tartgetRect.setWidth(width());
                    tartgetRect.setHeight(JOB_ITEM_TOTAL_HEIGHT);
                    m_animationManager.AddAnimation(m_jobItems[j], UIAnimationManager::ActionType_Geometry, m_jobItems[j]->geometry() , tartgetRect);
                    for(j = i+1; j < m_jobItems.count(); j++) {
                        tartgetRect.setTopLeft(QPoint(0, j * (JOB_ITEM_HEAD_HEIGHT + BORDER_SPACE) + BORDER_SPACE + JOB_ITEM_BODY_HEIGHT));
                        tartgetRect.setWidth(width());
                        tartgetRect.setHeight(JOB_ITEM_HEAD_HEIGHT);
                        m_animationManager.AddAnimation(m_jobItems[j], UIAnimationManager::ActionType_Geometry, m_jobItems[j]->geometry() , tartgetRect);
                    }

                    m_animationManager.AddAnimation(&m_itemsPane, UIAnimationManager::ActionType_Size, m_itemsPane.size(), QSize(width(), (m_jobItems.count()) * (JOB_ITEM_HEAD_HEIGHT + BORDER_SPACE) + BORDER_SPACE + JOB_ITEM_BODY_HEIGHT));
                }
                m_animationManager.StartAnimation();
            }
                break;

            default:
                break;
            }

            break;
        }
    }

    return obj->event(e);
}
