#include "itembase.h"

ItemBase::ItemBase(QWidget *parent, Qt::WindowFlags flags)
:QWidget(parent, flags)
,m_GraphicsOpacityEffect(this)
{

}

ItemBase::~ItemBase()
{

}

void ItemBase::setOpacity(qreal opacity)
{
	m_opacity = opacity;
	m_GraphicsOpacityEffect.setOpacity(m_opacity);
	setGraphicsEffect(&m_GraphicsOpacityEffect);
}

void ItemBase::SetOpacityMask(QColor color)
{
	m_GraphicsOpacityEffect.setOpacityMask(QBrush(color));
	setGraphicsEffect(&m_GraphicsOpacityEffect);
}
