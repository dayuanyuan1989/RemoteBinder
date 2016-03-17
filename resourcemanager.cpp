#include "resourcemanager.h"
#include <QSettings>

ResourceManager::ResourceManager(QObject *parent) : QObject(parent)
  , m_extension(false)
{

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::FileDataInfoInit()
{
    QSettings settings("configure.ini", QSettings::IniFormat);
    m_extension = settings.value("extension").toBool();
}

void ResourceManager::ResourceInit()
{
    // Parser Icons
    QList<QImage> paneIconList;
    paneIconList += CommonUtils::ParserIcons(":/variantsets/Resource/variantsets/btn_help@2x.png", QSize(48, 48));   // 2 pixmap
    paneIconList += QImage(":/variantsets/Resource/variantsets/btn_split_control@2x.png").copy(QRect(12, 5, 72, 38));
    paneIconList += QImage(":/variantsets/Resource/variantsets/btn_split_control@2x.png").copy(QRect(93, 5, 72, 38));
    paneIconList += QImage(":/variantsets/Resource/variantsets/btn_split_control@2x.png").copy(QRect(182, 5, 72, 38));
    paneIconList += QImage(":/variantsets/Resource/variantsets/btn_split_control@2x.png").copy(QRect(12, 53, 72, 38));
    paneIconList += QImage(":/variantsets/Resource/variantsets/btn_split_control@2x.png").copy(QRect(93, 53, 72, 38));
    paneIconList += QImage(":/variantsets/Resource/variantsets/btn_split_control@2x.png").copy(QRect(182, 53, 72, 38)); //(2+6) pixmap
    paneIconList += CommonUtils::ParserIcons(":/variantsets/Resource/variantsets/how-to@2x.png", QSize(640/3, 400)); // (8+3) pixmap
    paneIconList += CommonUtils::ParserIcons(":/variantsets/Resource/variantsets/icons_sensitivity_laser_size@2x.png", QSize(252/3, 84)); // (3+11) pximap
    paneIconList += QImage(":/variantsets/Resource/variantsets/slider_-button_2x.png"); // 15pixmap;
    paneIconList += CommonUtils::ParserIcons(":/variantsets/Resource/variantsets/btn_menu_filter@2x.png", QSize(48, 48)); //(15+4)pixmap
    paneIconList += QImage(":/variantsets/Resource/variantsets/aboutme.png");    // 20
    paneIconList += QImage(":/variantsets/Resource/variantsets/icon.png");   // 21
    ResourceManagerInst.SetTouchIcons(paneIconList);

    // parser icons
    ResourceManagerInst.SetMenuIcons(CommonUtils::ParserIcons(":/variantsets/Resource/variantsets/icons_variantsets.png", QSize(84, 84)));
    ResourceManagerInst.SetItemIcons(CommonUtils::ParserIcons(":/variantsets/Resource/variantsets/icons_variantsets.png", QSize(84, 84), QSize(74, 74)));
}

void ResourceManager::UpdateVariantSets(VarTree varTree, int index)
{
    if(index >= m_treeList.count()) return;
    m_treeList.replace(index, varTree);
}
