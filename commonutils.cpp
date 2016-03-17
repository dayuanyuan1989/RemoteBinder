#include "commonutils.h"
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

QList<QImage> CommonUtils::ParserIcons(const QString& picPath, const QSize& size, const QSize& innerSize)
{
    QList<QImage> list;

    QImage sourceicon(picPath);

    const int sw = size.width();
    const int sh = size.height();
    if( (sw == 0) || (sh == 0) || (sw > sourceicon.width()) || (sh > sourceicon.height()) \
            || (size.width() < innerSize.width()) || (size.height() < innerSize.height()) ) {
        return list;
    }

    const int row = sourceicon.width() / sw;
    const int col = sourceicon.height() / sh;

    int snapwidth;
    int snapheight;
    int snapDeltaX;
    int snapDeltaY;

    if( (innerSize.width() == 0) && (innerSize.height() == 0) ) {
        snapwidth = sw;
        snapheight = sh;
        snapDeltaX = 0;
        snapDeltaY = 0;
    } else {
        snapwidth = innerSize.width();
        snapheight = innerSize.height();
        snapDeltaX = (sw - snapwidth) /  2;
        snapDeltaY = (sh - snapheight) / 2;
    }

    QRect cpyRect;
    int i = 0;
    int j = 0;
    for(i = 0; i < col; i++) {
        cpyRect.setY(i * sh + snapDeltaY);
        for(j = 0; j < row; j++) {
            cpyRect.setX(j * sw + snapDeltaX);
            cpyRect.setWidth(snapwidth);
            cpyRect.setHeight(snapheight);
            list.append(sourceicon.copy(cpyRect));
        }
    }

    return list;
}

QList<VarTree> CommonUtils::ParserVariantSets(const QString& data)
{
    QList<VarTree> m_list;

    if(!data.contains("VariantSets")) return m_list;
    if(data.contains("VariantSets{};")) return m_list;

    int beginLen = QString("VariantSets").length() + data.indexOf("VariantSets");

    QString cpyData(data);
    QString variantsetsStr = cpyData.mid(beginLen + 1, cpyData.length() - (beginLen+1) - 3);
    variantsetsStr = variantsetsStr.trimmed();

    QList<QString> list = variantsetsStr.split(";");

    foreach (QString data, list) {
        bool rst = false;
        VarTree varTree = ParserVariantSet(data, rst);
        if(rst == true) {
            m_list.append(varTree);
        }
    }

    return m_list;
}

VarTree CommonUtils::ParserVariantSet(const QString& data, bool& rst)
{
    VarTree varTree;

    if(data.length() == 0) {
        rst = false;
        return varTree;
    }

    QString cpyStr(data);
    cpyStr = cpyStr.trimmed();
    cpyStr = cpyStr.mid(1, cpyStr.length()-2);

    QList<QString> list = cpyStr.split(":");
    QList<QString> varSetParam = list[0].split(",");
    QList<QString> varValue = list[1].split(",");
    // if varValues is "" empty string, the result of split is "", the list's count will be 1, but in fact, the list shuold be 0
    if(varValue.count()==1&&varValue[0].isEmpty()) varValue.clear();

    if( (list.count() != 2) || varSetParam.count() != 4 ) {
        rst = false;
        return varTree;
    }

    varTree.varTreeName = varSetParam[0];
    varTree.type = varSetParam[1].toULong();
    varTree.visible = varSetParam[2].toUInt() ? true : false;
    varTree.currentVarValue = varSetParam[3];
    varTree.varTreeValue = varValue;

    rst = true;
    return varTree;
}

CommonUtils::IconId CommonUtils::GetIconId(long type)
{
    IconId id = IconId_Count;

    switch (type) {
    case 0x00000100:
        id = IconId_Variant;
        break;
    case 0x00000004:
        id = IconId_Geometry;
        break;
    case 0x00000008:
        id = IconId_Material;
        break;
    case 0x00010000:
        id = IconId_Object_Set;
        break;
    case 0x00000040:
        id = IconId_Snapshot;
        break;
    case 0x00000080:
        id = IconId_Camera;
        break;
    default:
        break;
    }

    return id;
}

void CommonUtils::ShowTabTip()
{
    QDesktopServices::openUrl(QUrl("TabTip.exe", QUrl::TolerantMode));
}
