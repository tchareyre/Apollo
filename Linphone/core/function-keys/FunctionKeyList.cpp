#include "FunctionKeyList.hpp"

#include "FunctionKeyCore.hpp"
#include "core/App.hpp"
#include "model/core/CoreModel.hpp"

// =============================================================================

DEFINE_ABSTRACT_OBJECT(FunctionKeyList)

QSharedPointer<FunctionKeyList> FunctionKeyList::create() {
	auto model = QSharedPointer<FunctionKeyList>(new FunctionKeyList(), &QObject::deleteLater);
	model->moveToThread(App::getInstance()->thread());
	model->setSelf(model);
	return model;
}

FunctionKeyList::FunctionKeyList(QObject *parent) : ListProxy(parent) {
	mustBeInMainThread(getClassName());
	App::getInstance()->mEngine->setObjectOwnership(this, QQmlEngine::CppOwnership);
}

FunctionKeyList::~FunctionKeyList() {
	mustBeInMainThread("~" + getClassName());
	mModelConnection = nullptr;
}

void FunctionKeyList::setSelf(QSharedPointer<FunctionKeyList> me) {
	mModelConnection = SafeConnection<FunctionKeyList, CoreModel>::create(me, CoreModel::getInstance());

	mModelConnection->makeConnectToModel(&CoreModel::functionKeysFetched, [this](QList<FunctionKeyData> keys) {
		mModelConnection->invokeToCore([this, keys]() {
			mustBeInMainThread(Q_FUNC_INFO);
			beginResetModel();
			mList.clear();
			for (const auto &data : keys) {
				mList.append(QSharedPointer<FunctionKeyCore>(new FunctionKeyCore(data), &QObject::deleteLater));
			}
			endResetModel();
		});
	});

	mModelConnection->makeConnectToModel(&CoreModel::blfStateChanged,
	                                     [this](QString target, LinphoneEnums::BlfState state) {
		                                     mModelConnection->invokeToCore([this, target, state]() {
			                                     mustBeInMainThread(Q_FUNC_INFO);
			                                     for (auto &item : mList) {
				                                     auto key = item.objectCast<FunctionKeyCore>();
				                                     if (key && key->isBlf() && key->mTarget == target)
					                                     key->setBlfState(state);
			                                     }
		                                     });
	                                     });
}
