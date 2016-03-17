#include "uianimationmanager.h"

UIAnimationManager::UIAnimationManager()
:m_duration(ANIMATION_DURING_TIME)
,m_animType(QEasingCurve::Linear)
{

}

UIAnimationManager::~UIAnimationManager()
{
	RemoveAllAnimation();
}

void UIAnimationManager::AddAnimation(QWidget* pWidget, ActionType action, QVariant startValue, QVariant endValue)
{
    if(pWidget == NULL || OnRunning())  return;

    AnimationData data;
	data.p_widget = pWidget;
	data.p_prop_animation = new QPropertyAnimation(pWidget, GetPropertyName(action));
	data.action_type = action;
	data.start_value = startValue;
	data.end_value = endValue;

	connect(data.p_prop_animation, SIGNAL(finished()), this, SLOT(ReleaseAnimation()));
    connect(data.p_prop_animation, SIGNAL(valueChanged(QVariant)), this, SIGNAL(valueHasChanged(QVariant)));

	m_animList.append(data);
}
#if 0
void UIAnimationManager::InteruptAnimation(QWidget* pWidget, ActionType action)
{
    int i = 0;
    for(i = 0; i < m_animList.count(); i++) {
        if( (m_animList[i].p_widget == pWidget) && (m_animList[i].action_type == action) && m_animList[i].p_prop_animation->state() == QAbstractAnimation::Running ) {
            m_animList[i].p_prop_animation->stop();
            m_animList.removeAt(i);
            break;
        }
    }
}
#endif
void UIAnimationManager::StartAnimation()
{
	if(OnRunning()) return;

	int i = 0;
	for(i = 0; i < m_animList.count(); i++) {
		// duration
		m_animList[i].p_prop_animation->setDuration(m_duration);
        // animation type
        m_animList[i].p_prop_animation->setEasingCurve(m_animType);
		// start -- end value
        QVariant::Type type = m_animList[i].start_value.type();
		switch(type)
		{
        case QVariant::Int:
            m_animList[i].p_prop_animation->setStartValue(m_animList[i].start_value.toInt());
            m_animList[i].p_prop_animation->setEndValue(m_animList[i].end_value.toInt());
            break;
		case QVariant::Point:
			m_animList[i].p_prop_animation->setStartValue(m_animList[i].start_value.toPoint());
			m_animList[i].p_prop_animation->setEndValue(m_animList[i].end_value.toPoint());
			break;
		case QVariant::Size:
			m_animList[i].p_prop_animation->setStartValue(m_animList[i].start_value.toSize());
			m_animList[i].p_prop_animation->setEndValue(m_animList[i].end_value.toSize());
			break;
		case QVariant::Rect:
			m_animList[i].p_prop_animation->setStartValue(m_animList[i].start_value.toRect());
			m_animList[i].p_prop_animation->setEndValue(m_animList[i].end_value.toRect());
			break;
		case QVariant::Double:
			m_animList[i].p_prop_animation->setStartValue(m_animList[i].start_value.toFloat());
			m_animList[i].p_prop_animation->setEndValue(m_animList[i].end_value.toFloat());
			break;
		default:
			break;
		}
		// to start
		m_animList[i].p_prop_animation->start();
	}
}

void UIAnimationManager::ReleaseAnimation()
{
	static int counter = 0;

	counter++;

	if(counter == m_animList.count()) {
		counter = 0;
		
		RemoveAllAnimation();
	}
}

void UIAnimationManager::RemoveAllAnimation() 
{
	int i = 0;
	for(i = 0; i < m_animList.count(); i++) {
		disconnect(m_animList[i].p_prop_animation, SIGNAL(finished()), this, SLOT(ReleaseAnimation()));
        disconnect(m_animList[i].p_prop_animation, SIGNAL(valueChanged(QVariant)), this, SIGNAL(valueHasChanged(QVariant)));
		delete m_animList[i].p_prop_animation;
		m_animList[i].p_prop_animation = NULL;
	}

	m_animList.clear();

	emit finished();
}

QByteArray UIAnimationManager::GetPropertyName(ActionType type) 
{
	QByteArray str;

	switch(type)
	{
	case ActionType_Pos:
		str = QByteArray("pos");
		break;
	case ActionType_Size:
		str = QByteArray("size");
		break;
	case ActionType_Geometry:
		str = QByteArray("geometry");
		break;
	case ActionType_Opacity:
		str = QByteArray("opacity");
		break;
    case ActionType_ScrollBar_Value:
        str = QByteArray("value");
        break;
	default:
		break;
	}

	return str;
}

bool UIAnimationManager::OnRunning()
{
	int i = 0;
	for(i = 0; i < m_animList.count(); i++) {
		if(m_animList[i].p_prop_animation->state() != QAbstractAnimation::Running) {
			return false;
		}
	}

	return (m_animList.count() > 0 ? true : false);
}
