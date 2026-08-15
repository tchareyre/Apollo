#include "FunctionKeysModel.hpp"

#include "BlfSubscription.hpp"
#include "model/core/CoreModel.hpp"
#include "tool/Utils.hpp"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSet>
#include <algorithm>

// =============================================================================

DEFINE_ABSTRACT_OBJECT(FunctionKeysModel)

// No native liflinphone resync cadence to mirror here (unlike the vCard
// FriendList, which liblinphone itself keeps fresh) -- this interval is a
// judgment call, not copied from an existing precedent in this app.
static constexpr int RefetchIntervalMs = 45 * 60 * 1000;

FunctionKeysModel::FunctionKeysModel(QObject *parent) : QObject(parent) {
	connect(CoreModel::getInstance().get(), &CoreModel::accountRegistrationStateChanged, this,
	        [this](const std::shared_ptr<linphone::Core> &, const std::shared_ptr<linphone::Account> &,
	               linphone::RegistrationState state, const std::string &) {
		        if (state == linphone::RegistrationState::Failed || state == linphone::RegistrationState::Cleared) {
			        // A registration that was actually up just went away (real
			        // network drop, or -- as seen in testing -- the throwaway
			        // first Core used by `--fetch-config` being torn down after
			        // applying the fetched config). Either way, whatever this
			        // model's subscriptions are bound to is no longer valid.
			        mNeedsResubscribeOnReconnect = true;
		        } else if (state == linphone::RegistrationState::Ok) {
			        if (mNeedsResubscribeOnReconnect && !mSubscriptions.isEmpty()) resubscribeAll();
			        mNeedsResubscribeOnReconnect = false;
		        }
	        });
	connect(&mRefetchTimer, &QTimer::timeout, this, [this]() { fetch(mUrl); });
}

FunctionKeysModel::~FunctionKeysModel() {
	for (auto &sub : mSubscriptions)
		sub->terminate();
	mSubscriptions.clear();
}

void FunctionKeysModel::fetch(const QString &url) {
	if (url.isEmpty()) return;
	mUrl = url;
	if (!mNam) mNam = new QNetworkAccessManager(this);
	// Abort any still-in-flight request first -- liblinphone can trigger
	// more than one ConfiguringState::Successful in the same run (e.g. the
	// persisted config-uri auto-reapplied at startup plus an explicit
	// --fetch-config), and two concurrent GETs racing to applyKeys() would
	// otherwise each see an empty mSubscriptions and both create a BLF
	// SUBSCRIBE for the same target (reproduced: two separate dialogs on
	// the same resource in `pjsip show subscriptions` after a real test).
	if (mPendingReply) {
		mPendingReply->abort();
		mPendingReply->deleteLater();
		mPendingReply = nullptr;
	}
	QNetworkRequest request{QUrl(url)};
	mPendingReply = mNam->get(request);
	connect(mPendingReply, &QNetworkReply::finished, this, &FunctionKeysModel::onReplyFinished);
	if (!mRefetchTimer.isActive()) mRefetchTimer.start(RefetchIntervalMs);
}

void FunctionKeysModel::onReplyFinished() {
	auto reply = dynamic_cast<QNetworkReply *>(sender());
	if (!reply) return;
	if (reply == mPendingReply) mPendingReply = nullptr;
	reply->deleteLater();

	if (reply->error() != QNetworkReply::NoError) {
		lWarning() << log().arg("Function keys fetch failed:").arg(reply->errorString());
		return; // Transient failure: keep whatever key list/subscriptions we already had.
	}

	auto document = QJsonDocument::fromJson(reply->readAll());
	if (!document.isArray()) {
		lWarning() << log().arg("Function keys reply is not a JSON array");
		return;
	}

	QList<FunctionKeyData> keys;
	for (const auto &item : document.array()) {
		auto obj = item.toObject();
		FunctionKeyData key;
		key.position = obj.value("position").toInt();
		key.type = obj.value("type").toString();
		key.target = obj.value("target").toString();
		key.label = obj.value("label").toString();
		// Fall back to the bare target: an Sidus older than these two fields
		// still serves usable keys, and that is exactly what this app did
		// before they existed.
		key.dialTarget = obj.value("dialTarget").toString(key.target);
		key.pickupTarget = obj.value("pickupTarget").toString();
		keys.append(key);
	}
	std::sort(keys.begin(), keys.end(),
	          [](const FunctionKeyData &a, const FunctionKeyData &b) { return a.position < b.position; });

	applyKeys(keys);
}

void FunctionKeysModel::applyKeys(const QList<FunctionKeyData> &keys) {
	mKeys = keys;

	QSet<QString> blfTargets;
	for (const auto &key : keys) {
		if (key.type == QLatin1String("blf")) blfTargets.insert(key.target);
	}

	// Drop subscriptions whose target is no longer a BLF key. Explicit
	// terminate() first, never a bare drop -- see BlfSubscription's own
	// comment: an untracked shared_ptr release alone can leave an orphaned
	// SUBSCRIBE dialog on Asterisk's side.
	for (auto it = mSubscriptions.begin(); it != mSubscriptions.end();) {
		if (!blfTargets.contains(it.key())) {
			it.value()->terminate();
			it = mSubscriptions.erase(it);
		} else {
			++it;
		}
	}

	// Add subscriptions for newly-seen BLF targets only -- targets already
	// subscribed are left alone so a routine re-fetch (the periodic timer
	// above) never churns an already-healthy subscription.
	auto core = CoreModel::getInstance()->getCore();
	for (const auto &target : blfTargets) {
		if (mSubscriptions.contains(target)) continue;
		auto sub = BlfSubscription::create(core, target);
		if (!sub) continue;
		connect(sub.get(), &BlfSubscription::stateChanged, this, &FunctionKeysModel::blfStateChanged);
		mSubscriptions.insert(target, sub);
	}

	emit keysFetched(mKeys);
}

void FunctionKeysModel::resubscribeAll() {
	// Reconnect resilience: a transport/network loss can tear down SUBSCRIBE
	// dialogs without a clean onSubscribeStateChanged, so rebuild every BLF
	// subscription from scratch once the default account recovers rather
	// than trusting any surviving state. terminate() defensively first --
	// even on a real outage this is a harmless no-op on an already-dead
	// dialog, and it's the only thing that prevents a duplicate dialog on
	// Asterisk's side if the old one actually survived.
	auto targets = mSubscriptions.keys();
	for (auto &sub : mSubscriptions)
		sub->terminate();
	mSubscriptions.clear();
	auto core = CoreModel::getInstance()->getCore();
	for (const auto &target : targets) {
		auto sub = BlfSubscription::create(core, target);
		if (!sub) continue;
		connect(sub.get(), &BlfSubscription::stateChanged, this, &FunctionKeysModel::blfStateChanged);
		mSubscriptions.insert(target, sub);
	}
}
