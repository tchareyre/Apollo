#ifndef BLF_SUBSCRIPTION_H_
#define BLF_SUBSCRIPTION_H_

#include "model/listener/Listener.hpp"
#include "tool/AbstractObject.hpp"
#include "tool/LinphoneEnums.hpp"
#include <QObject>
#include <QString>
#include <linphone++/linphone.hh>

// =============================================================================

// One instance per watched BLF target extension: owns the SIP SUBSCRIBE to
// Asterisk's "dialog" event package (RFC 4235) for that extension's hint
// (already wired server-side via subscribe_context/hint for every Orbit
// extension, same as a hardware Fanvil phone doing BLF), and parses the
// resulting NOTIFY bodies via DialogInfoParser. Owned exclusively by
// FunctionKeysModel, which lives on the model/Linphone thread -- never
// touch this class from the QML thread.
class BlfSubscription : public ::Listener<linphone::Event, linphone::EventListener>,
                        public linphone::EventListener,
                        public AbstractObject {
	Q_OBJECT
public:
	BlfSubscription(const std::shared_ptr<linphone::Event> &event, const QString &target, QObject *parent = nullptr);

	// Returns nullptr if the target can't be interpreted as a dialable
	// address (mirrors ToolModel::createCall's own failure mode).
	static std::shared_ptr<BlfSubscription>
	create(const std::shared_ptr<linphone::Core> &core, const QString &target, QObject *parent = nullptr);

	QString getTarget() const {
		return mTarget;
	}

	// Explicit SUBSCRIBE teardown -- must be called before dropping the
	// last reference to this object (e.g. on a function-keys refresh that
	// removes/changes this target), otherwise Asterisk is left holding an
	// orphaned SUBSCRIBE dialog with nobody listening.
	void terminate();

signals:
	void stateChanged(QString target, LinphoneEnums::BlfState state);

private:
	virtual void onNotifyReceived(const std::shared_ptr<linphone::Event> &event,
	                              const std::shared_ptr<const linphone::Content> &content) override;
	virtual void onSubscribeStateChanged(const std::shared_ptr<linphone::Event> &event,
	                                     linphone::SubscriptionState state) override;

	QString mTarget;

	DECLARE_ABSTRACT_OBJECT
};

#endif
