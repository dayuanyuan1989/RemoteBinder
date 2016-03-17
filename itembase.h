#ifndef ITEM_BASE_H
#define ITEM_BASE_H

#include <QWidget>
#include <QGraphicsOpacityEffect>

class ItemBase : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)
public:
    enum Status { ButtonStatus_Normal, ButtonStatus_Entry, ButtonStatus_Pressed,  ButtonStatus_NotMePressed };

    ItemBase(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~ItemBase();

    virtual void SetStatus( Status status) { m_status = status; }
    Status GetStatus() { return m_status; }

    qreal getOpacity() const { return m_opacity; }
	void setOpacity(qreal opacity);

protected:
	void SetOpacityMask(QColor color);

private:
    Status                  m_status;
	// Opacity
	QGraphicsOpacityEffect	m_GraphicsOpacityEffect;
	qreal					m_opacity;

signals:
    void MouseButtonClicked();
};

#endif // ITEM_BASE_H
