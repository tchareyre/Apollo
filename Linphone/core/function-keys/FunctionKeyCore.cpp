#include "FunctionKeyCore.hpp"

#include "core/App.hpp"
#include "tool/Utils.hpp"

// =============================================================================

DEFINE_ABSTRACT_OBJECT(FunctionKeyCore)

FunctionKeyCore::FunctionKeyCore(const FunctionKeyData &data, QObject *parent)
    : QObject(parent), mPosition(data.position), mType(data.type), mTarget(data.target), mLabel(data.label) {
	App::getInstance()->mEngine->setObjectOwnership(this, QQmlEngine::CppOwnership);
}

void FunctionKeyCore::call() const {
	Utils::createCall(mTarget);
}

void FunctionKeyCore::setBlfState(LinphoneEnums::BlfState state) {
	if (mBlfState != state) {
		mBlfState = state;
		emit blfStateChanged();
	}
}
