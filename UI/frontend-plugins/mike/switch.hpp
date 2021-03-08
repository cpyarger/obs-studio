#pragma once

// THIS CODE WAS GRABBED FROM THE ANSWER AT THIS STACKOVERFLOW LINK:
// https://stackoverflow.com/questions/14780517/toggle-switch-in-qt

#include <QtWidgets>
#include "style.hpp"

class Animator final : public QVariantAnimation {
	Q_OBJECT
	Q_PROPERTY(QObject *targetObject READ targetObject WRITE setTargetObject)

public:
	Animator(QObject *target, QObject *parent = nullptr);
	~Animator() override;

	QObject *targetObject() const;
	void setTargetObject(QObject *target);

	inline bool isRunning() const { return state() == Running; }

public slots:
	void setup(int duration, QEasingCurve easing = QEasingCurve::Linear);
	void interpolate(const QVariant &start, const QVariant &end);
	void setCurrentValue(const QVariant &);

protected:
	void updateCurrentValue(const QVariant &value) override final;
	void updateState(QAbstractAnimation::State newState,
			 QAbstractAnimation::State oldState) override final;

private:
	QPointer<QObject> target;
};

class SelectionControl : public QAbstractButton {
	Q_OBJECT

public:
	explicit SelectionControl(QWidget *parent = nullptr);
	~SelectionControl() override;

	Qt::CheckState checkState() const;

Q_SIGNALS:
	void stateChanged(int);

protected:
	void enterEvent(QEvent *) override;
	void checkStateSet() override;
	void nextCheckState() override;
	virtual void toggle(Qt::CheckState state) = 0;
};

class Switch final : public SelectionControl {
	Q_OBJECT

	static constexpr auto CORNER_RADIUS = 8.0;
	static constexpr auto THUMB_RADIUS = 14.5;
	static constexpr auto SHADOW_ELEVATION = 2.0;

public:
	bool isChecked();
	explicit Switch(QWidget *parent = nullptr);
	Switch(const QString &text, QWidget *parent = nullptr);
	Switch(const QString &text, const QBrush &, QWidget *parent = nullptr);
	~Switch() override;

	QSize sizeHint() const override final;

protected:
	void paintEvent(QPaintEvent *) override final;
	void resizeEvent(QResizeEvent *) override final;
	void toggle(Qt::CheckState) override final;

	void init();
	QRect indicatorRect();
	QRect textRect();

	static inline QColor colorFromOpacity(const QColor &c, qreal opacity)
	{
		return QColor(c.red(), c.green(), c.blue(),
			      qRound(opacity * 255.0));
	}
	static inline bool ltr(QWidget *w)
	{
		if (nullptr != w)
			return w->layoutDirection() == Qt::LeftToRight;

		return false;
	}

private:
	std::function<void()> cb;
	Style::Switch style;
	QPixmap shadowPixmap;
	QPointer<Animator> thumbBrushAnimation;
	QPointer<Animator> trackBrushAnimation;
	QPointer<Animator> thumbPosAniamtion;
};
