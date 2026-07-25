#include "DialogInfoParser.hpp"

#include <QXmlStreamReader>

// =============================================================================

namespace {

LinphoneEnums::BlfState stateFromDialogState(const QString &state) {
	if (state == QLatin1String("confirmed")) return LinphoneEnums::BlfState::Busy;
	if (state == QLatin1String("early") || state == QLatin1String("proceeding") || state == QLatin1String("trying"))
		return LinphoneEnums::BlfState::Ringing;
	return LinphoneEnums::BlfState::Idle; // "terminated" and anything unrecognized
}

} // namespace

LinphoneEnums::BlfState DialogInfoParser::parse(const QByteArray &xml) {
	QXmlStreamReader reader(xml);
	bool sawDialog = false;
	LinphoneEnums::BlfState result = LinphoneEnums::BlfState::Idle;

	while (!reader.atEnd() && !reader.hasError()) {
		reader.readNext();
		if (!reader.isStartElement()) continue;

		if (reader.name() == QLatin1String("dialog")) {
			sawDialog = true;
		} else if (reader.name() == QLatin1String("state")) {
			auto mapped = stateFromDialogState(reader.readElementText().trimmed().toLower());
			// A dialog-info body can carry several <dialog> elements (e.g. a
			// call mid-transfer) -- keep the busiest state observed rather
			// than the last one in document order, so the lamp never shows
			// idle while one of the dialogs is actually confirmed/ringing.
			if (mapped == LinphoneEnums::BlfState::Busy) return LinphoneEnums::BlfState::Busy;
			if (mapped == LinphoneEnums::BlfState::Ringing && result == LinphoneEnums::BlfState::Idle) {
				result = LinphoneEnums::BlfState::Ringing;
			}
		}
	}

	if (!sawDialog) return LinphoneEnums::BlfState::Idle;
	return result;
}
