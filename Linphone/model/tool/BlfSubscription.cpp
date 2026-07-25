#include "BlfSubscription.hpp"

#include "DialogInfoParser.hpp"
#include "model/tool/ToolModel.hpp"
#include "tool/Utils.hpp"

// =============================================================================

DEFINE_ABSTRACT_OBJECT(BlfSubscription)

static constexpr int BlfSubscribeExpiresSeconds = 600;

BlfSubscription::BlfSubscription(const std::shared_ptr<linphone::Event> &event, const QString &target, QObject *parent)
    : ::Listener<linphone::Event, linphone::EventListener>(event, parent), mTarget(target) {
}

std::shared_ptr<BlfSubscription>
BlfSubscription::create(const std::shared_ptr<linphone::Core> &core, const QString &target, QObject *parent) {
	auto address = ToolModel::interpretUrl(target);
	if (!address) {
		lWarning() << "[BlfSubscription] Cannot interpret BLF target as a dialable address:" << target;
		return nullptr;
	}
	auto event = core->createSubscribe(address, "dialog", BlfSubscribeExpiresSeconds);
	if (!event) {
		lWarning() << "[BlfSubscription] core->createSubscribe returned null for target:" << target;
		return nullptr;
	}
	auto sub = std::make_shared<BlfSubscription>(event, target, parent);
	sub->setSelf(sub);
	event->sendSubscribe(nullptr);
	return sub;
}

void BlfSubscription::terminate() {
	if (mMonitor) mMonitor->terminate();
}

void BlfSubscription::onNotifyReceived(const std::shared_ptr<linphone::Event> &event,
                                       const std::shared_ptr<const linphone::Content> &content) {
	if (!content) return;
	auto xml = QByteArray::fromStdString(content->getUtf8Text());
	auto state = DialogInfoParser::parse(xml);
	emit stateChanged(mTarget, state);
}

void BlfSubscription::onSubscribeStateChanged(const std::shared_ptr<linphone::Event> &event,
                                              linphone::SubscriptionState state) {
	// Error/Terminated outside of our own explicit terminate() means the
	// dialog is gone and the lamp can no longer be trusted -- surface it
	// rather than leaving the last-known state displayed indefinitely.
	// Reconnection is handled at the FunctionKeysModel level (it rebuilds
	// every subscription once the account re-registers), not here.
	if (state == linphone::SubscriptionState::Error || state == linphone::SubscriptionState::Terminated) {
		emit stateChanged(mTarget, LinphoneEnums::BlfState::Unavailable);
	}
}
