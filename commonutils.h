#ifndef ICONPARSER_H
#define ICONPARSER_H

#include <QObject>
#include <QImage>
#include <QStringList>

typedef struct{ unsigned long type; QString varTreeName; QList<QString> varTreeValue; QString currentVarValue;bool visible; } VarTree;

class CommonUtils : public QObject
{
    Q_OBJECT
public:
    enum IconId { IconId_Variant, IconId_Geometry, IconId_Material, IconId_Object_Set=4, IconId_Snapshot, IconId_Camera, IconId_Count = 12 };

    static QList<QImage> ParserIcons(const QString& picPath, const QSize& size, const QSize& innerSize = QSize(0, 0));
    static QList<VarTree> ParserVariantSets(const QString& data);
    static IconId GetIconId(long type);
    static void ShowTabTip();
private:
    static VarTree ParserVariantSet(const QString& data, bool& rst);
};

#endif // ICONPARSER_H
