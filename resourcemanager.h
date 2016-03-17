#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QObject>
#include <QList>
#include <QImage>
#include <QString>
#include <commonutils.h>

#if defined(UNICODE)
#define TITLE_NAME  L"CPlore"
#else
#define TITLE_NAME  "CPlore"
#endif

typedef struct {
    QString server;
    uint    port;
    QString title;
} LoginInfo, *P2LoginInfo;

typedef struct {
    QString title;
    QString address;
    uint    port;
    QString user;
    QString password;
} PbInfo, *P2PbInfo;

class ResourceManager : public QObject
{
    Q_OBJECT
public:
    static ResourceManager& GetInstance()
    {
        static ResourceManager instance;
        return instance;
    }

    void FileDataInfoInit();
    void ResourceInit();

    void UpdateVariantSets(VarTree, int);
    void SetVariantSets(const QList<VarTree> &list) { m_treeList = list; }

    bool isShowOnExtensionScreen() const { return m_extension; }

    QList<VarTree> GetVariantSets() {return m_treeList; }
    QList<QImage> GetMenuIcons() { return m_iconMenuList; }
    QList<QImage> GetItemIcons() { return m_iconItemList; }
    QList<QImage> GetTouchIcons() { return m_iconTouchList; }

private:
    void SetMenuIcons(const QList<QImage> &list) { m_iconMenuList = list; }
    void SetItemIcons(const QList<QImage> &list) { m_iconItemList = list; }
    void SetTouchIcons(const QList<QImage> &list) { m_iconTouchList = list; }

private:
    explicit ResourceManager(QObject *parent = 0);
    ResourceManager(const ResourceManager&) : QObject(NULL) {}
    ResourceManager& operator=(const ResourceManager &);
    virtual ~ResourceManager();

public:
    QList<LoginInfo>        loginInfoList;
    QList<PbInfo>           pbInfoList;

private:
    QList<VarTree>          m_treeList;
    QList<QImage>           m_iconMenuList;
    QList<QImage>           m_iconItemList;
    QList<QImage>           m_iconTouchList;

    bool                    m_extension;

signals:

public slots:
};

#define ResourceManagerInst ResourceManager::GetInstance()
#define VariantSetsList     (ResourceManagerInst.GetVariantSets())
#define MenuIconsList       (ResourceManagerInst.GetMenuIcons())
#define ItemIconsList       (ResourceManagerInst.GetItemIcons())
#define TouchIconList       (ResourceManagerInst.GetTouchIcons())
#define LoginInfoList       (ResourceManagerInst.loginInfoList)
#define PbInfoList          (ResourceManagerInst.pbInfoList)

#endif // RESOURCEMANAGER_H
