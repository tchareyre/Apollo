#ifndef FUNCTION_KEYS_MODEL_H_
#define FUNCTION_KEYS_MODEL_H_

#include "tool/AbstractObject.hpp"
#include "tool/LinphoneEnums.hpp"
#include <QHash>
#include <QList>
#include <QNetworkAccessManager>
#include <QObject>
#include <QString>
#include <QTimer>
#include <linphone++/linphone.hh>
#include <memory>

class BlfSubscription;
class QNetworkReply;

// =============================================================================

// Plain record for one row of Orbit's function-keys.json (see
// backend/src/provisioningServer.js's function-keys.json route) -- position
// or type/target/label meanings mirror functionKeysStore.js on the Orbit
// side exactly (type is one of "blf"/"speedDial"/"callPark"/"callForward").
struct FunctionKeyData {
	int position = 0;
	QString type;
	QString target;
	QString label;
};
Q_DECLARE_METATYPE(FunctionKeyData)

// Fetches Apollo's function keys from the [misc] function_keys_url the
// org's remote-provisioning config points at (see
// CoreModel::syncFunctionKeys), and owns the live SIP BLF SUBSCRIBE
// (BlfSubscription) for every "blf"-type key. Lives on the model/Linphone
// thread, following the same plain QNetworkAccessManager GET pattern as
// OIDCModel (the only other place in the app doing this outside of
// liblinphone's own sync mechanisms) -- unlike the vCard directory sync,
// there's no native liblinphone sync type for this, so a QTimer drives
// periodic re-fetches too (function keys can change server-side without
// the user restarting the app or re-provisioning).
class FunctionKeysModel : public QObject, public AbstractObject {
	Q_OBJECT
public:
	FunctionKeysModel(QObject *parent = nullptr);
	~FunctionKeysModel();

	void fetch(const QString &url);

signals:
	void keysFetched(QList<FunctionKeyData> keys);
	void blfStateChanged(QString target, LinphoneEnums::BlfState state);

private:
	void onReplyFinished();
	void applyKeys(const QList<FunctionKeyData> &keys);
	void resubscribeAll();

	QNetworkAccessManager *mNam = nullptr;
	QNetworkReply *mPendingReply = nullptr;
	QTimer mRefetchTimer;
	QString mUrl;
	QList<FunctionKeyData> mKeys;
	QHash<QString /*target*/, std::shared_ptr<BlfSubscription>> mSubscriptions;
	// Edge-triggers resubscribeAll() only on a genuine "was registered, then
	// dropped, now back" cycle (set on Failed/Cleared, consumed on the next
	// Ok) -- NOT on every transition into Ok. The very first Ok of a launch
	// must NOT resubscribe: apollo --fetch-config runs a throwaway first
	// Core just to apply the fetched config, whose own initial
	// None->Progress->Ok would otherwise look identical to a real recovery
	// once compared against "previous state != Ok" (reproduced: an
	// unnecessary extra BLF SUBSCRIBE on that very first, perfectly normal
	// registration).
	bool mNeedsResubscribeOnReconnect = false;

	DECLARE_ABSTRACT_OBJECT
};

#endif
