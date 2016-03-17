#ifndef ADDJOBDIAG_H
#define ADDJOBDIAG_H

#include <QWidget>
#include <QMap>

class QSize;
class QString;
class QFont;

namespace Ui {
class AddJobDiag;
}

struct RenderData
{
public:
    RenderData() {}
    QString	fileName;
    QSize	size;
    ulong	format;		/* 0 - jpg, 1 - tif, 2 - png */
    ulong	quality;	/* 0 - opengl, 1 - real trace, 2 - global illumination */
} ;

class AddJobDiag : public QWidget
{
    Q_OBJECT

public:
    explicit AddJobDiag(QWidget *parent = 0);
    ~AddJobDiag();

    QByteArray GetAddRenderTaskString();

private:
    void init();
    void final();

    void SetFontSizeCombox(const QFont&);

signals:
    void AddNewTask(const RenderData&);

    void WaterMarkVisibleChanged(bool);
    void fontFamilyChanged(const QFont&);
    void fontSizeChanged(uint);
    void BoldStateChanged(bool);
    void ItalyStateChanged(bool);
    void TextColorChanged(const QColor&);
    void TextChanged(const QString&);

private slots:
    void on_addTaskBtn_clicked();

    void on_colorlineEdit_editingFinished();

    void on_fontsizecombox_currentIndexChanged(const QString &arg1);

    void on_textedit_textChanged();

    void on_fontComboBox_currentFontChanged(const QFont &f);

private:
    Ui::AddJobDiag *ui;

    QMap<QString, QSize> m_sizeMap;
};

#endif // ADDJOBDIAG_H
