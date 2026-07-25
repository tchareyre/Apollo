#ifndef FUNCTION_KEY_LIST_H_
#define FUNCTION_KEY_LIST_H_

#include "../proxy/ListProxy.hpp"
#include "tool/AbstractObject.hpp"
#include "tool/thread/SafeConnection.hpp"

class CoreModel;

// =============================================================================

// QML-facing list of FunctionKeyCore, ordered by position. Follows
// CallHistoryList's create()/setSelf()/SafeConnection pattern, but
// connects to CoreModel directly (not a bespoke model-thread singleton)
// since CoreModel already relays FunctionKeysModel's signals as its own
// (functionKeysFetched/blfStateChanged) -- the same "every list talks to
// CoreModel" convention every other list in this app follows. Uses
// ListProxy's default data() (a plain QObject* per row) rather than
// overriding it: function keys have no separate "Gui" wrapper layer, see
// FunctionKeyCore's own comment.
class FunctionKeyList : public ListProxy, public AbstractObject {
	Q_OBJECT
public:
	static QSharedPointer<FunctionKeyList> create();
	FunctionKeyList(QObject *parent = Q_NULLPTR);
	~FunctionKeyList();

	void setSelf(QSharedPointer<FunctionKeyList> me);

signals:
	void lUpdate();

private:
	QSharedPointer<SafeConnection<FunctionKeyList, CoreModel>> mModelConnection;
	DECLARE_ABSTRACT_OBJECT
};

#endif
