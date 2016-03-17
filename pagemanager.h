#ifndef PAGEMANAGER_H
#define PAGEMANAGER_H

#include <QList>

#include "loginpane.h"
#include "mainpane.h"

class PageManager : public QObject
{
    Q_OBJECT
public:
    enum PageId {
        PageId_Begin = 0,
        PageId_Login = PageId_Begin,
        PageId_LoginEx,
        PageId_MainPane,
        PageId_MainPaneEx,
        PageId_Count,
        PageInvalid = PageId_Count
    };

    static PageManager& GetInstance() {
        static PageManager instance;
        return instance;
    }

    virtual ~PageManager();

    void ShowPageInit(PageId = PageId_Begin);

private:
    explicit PageManager(QObject* parent = 0);
    PageManager(const PageManager&):QObject(NULL) {}
    PageManager& operator =(const PageManager&);

    void init();
    void final();

    void SaveUserInfo();
    void LoadUserInfo();

public slots:
//    void OnPageJump(PageId id);
    void OnPageJump(int idx);
    void OnConnectHandle(const QString&, uint);
    void OnQuit();

private:
    QList<QWidget*>       m_pageList;
    PageId                m_curPageId;
    QWidget*               m_pBaseWidget;
};

#define PageManagerInst PageManager::GetInstance()

#endif // PAGEMANAGER_H
