#include "FunctionKeyCore.hpp"

#include "core/App.hpp"
#include "tool/Utils.hpp"

// =============================================================================

DEFINE_ABSTRACT_OBJECT(FunctionKeyCore)

FunctionKeyCore::FunctionKeyCore(const FunctionKeyData &data, QObject *parent)
    : QObject(parent), mPosition(data.position), mType(data.type), mTarget(data.target), mLabel(data.label),
      mDialTarget(data.dialTarget.isEmpty() ? data.target : data.dialTarget), mPickupTarget(data.pickupTarget) {
	App::getInstance()->mEngine->setObjectOwnership(this, QQmlEngine::CppOwnership);
}

void FunctionKeyCore::call() const {
	// Ringing is the only state where the pickup code is the useful action:
	// idle or busy, it would either find nothing to pick up or steal a call
	// already in progress somewhere else in the org (Pickup() is scoped to
	// the endpoint's named_pickup_group, not to this key's extension).
	if (!mPickupTarget.isEmpty() && mBlfState == LinphoneEnums::BlfState::Ringing) {
		Utils::createCall(mPickupTarget);
		return;
	}
	// Parking is a hand-over, not a call: the point of the key is to send the
	// correspondent to a parking space, and Utils falls back to dialing when
	// there is no call in progress.
	if (mType == QLatin1String("callPark")) {
		Utils::transferCurrentCallTo(mDialTarget);
		return;
	}
	Utils::createCall(mDialTarget);
}

void FunctionKeyCore::setBlfState(LinphoneEnums::BlfState state) {
	if (mBlfState != state) {
		mBlfState = state;
		emit blfStateChanged();
	}
}
