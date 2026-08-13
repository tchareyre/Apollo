#ifndef FUNCTION_KEY_CORE_H_
#define FUNCTION_KEY_CORE_H_

#include "model/tool/FunctionKeysModel.hpp"
#include "tool/AbstractObject.hpp"
#include "tool/LinphoneEnums.hpp"
#include <QObject>
#include <QSharedPointer>
#include <QString>

// =============================================================================

// QML-facing wrapper for one Orbit function key (see FunctionKeyData). Kept
// as a single layer (no separate "Gui" wrapper like CallHistoryCore/
// CallHistoryGui) -- function keys are read-only display data plus one
// invokable action, so there's no per-view state that would justify the
// extra layer.
class FunctionKeyCore : public QObject, public AbstractObject {
	Q_OBJECT

	Q_PROPERTY(int position MEMBER mPosition CONSTANT)
	Q_PROPERTY(QString type MEMBER mType CONSTANT)
	Q_PROPERTY(QString target MEMBER mTarget CONSTANT)
	Q_PROPERTY(QString label MEMBER mLabel CONSTANT)
	Q_PROPERTY(bool isBlf READ isBlf CONSTANT)
	Q_PROPERTY(LinphoneEnums::BlfState blfState MEMBER mBlfState NOTIFY blfStateChanged)

public:
	FunctionKeyCore(const FunctionKeyData &data, QObject *parent = Q_NULLPTR);

	bool isBlf() const {
		return mType == QLatin1String("blf");
	}

	// One tap dials mDialTarget (see ToolModel::createCall, which resolves a
	// bare extension/number the same way the in-app dialer does). Which
	// digits that is comes from Orbit, not from here -- a "callForward" key
	// dials the enable-forwarding code plus its destination, not the
	// destination itself. A BLF key whose extension is currently ringing
	// dials mPickupTarget instead, so the press intercepts the call the way
	// a hardware BLF key does rather than adding a second ring.
	Q_INVOKABLE void call() const;

	void setBlfState(LinphoneEnums::BlfState state);

	int mPosition;
	QString mType;
	QString mTarget;
	QString mLabel;
	QString mDialTarget;
	QString mPickupTarget;
	LinphoneEnums::BlfState mBlfState = LinphoneEnums::BlfState::Idle;

signals:
	void blfStateChanged();

private:
	DECLARE_ABSTRACT_OBJECT
};
Q_DECLARE_METATYPE(FunctionKeyCore *)

#endif
