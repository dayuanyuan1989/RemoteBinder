#ifndef UI_ANIMATION_MANAGER_H
#define UI_ANIMATION_MANAGER_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QVariant>
#include <QList>
#include <QByteArray>

#define ANIMATION_DURING_TIME		(500)

class UIAnimationManager : public QObject
{
	Q_OBJECT

public:
    enum ActionType { ActionType_Pos, ActionType_Size , ActionType_Geometry , ActionType_Opacity, ActionType_ScrollBar_Value, ActionType_Invalid };
    typedef struct {QWidget* p_widget; QPropertyAnimation* p_prop_animation; ActionType action_type; QVariant start_value; QVariant end_value;} AnimationData;

	UIAnimationManager();
	~UIAnimationManager();

	void AddAnimation(QWidget* pWidget, ActionType action, QVariant startValue, QVariant endValue);
	void StartAnimation();

    void SetEasingCurve(const QEasingCurve::Type &easing) { m_animType = easing; }
    bool OnRunning();

	inline void SetDuration(int duration) { m_duration = duration; }
#if 0
    void InteruptAnimation(QWidget* pWidget, ActionType action);
#endif
    void RemoveAllAnimation();

private:
	QByteArray	GetPropertyName(ActionType type);

private:
	QList<AnimationData>		m_animList;
	int							m_duration;
    QEasingCurve::Type          m_animType;

public Q_SLOTS:
    void ReleaseAnimation();

Q_SIGNALS:
    void finished();
    void valueHasChanged(const QVariant &value);
};

#endif
