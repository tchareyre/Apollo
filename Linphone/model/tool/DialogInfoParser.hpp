#ifndef DIALOG_INFO_PARSER_H_
#define DIALOG_INFO_PARSER_H_

#include "tool/LinphoneEnums.hpp"
#include <QByteArray>

// Parses a SIP "dialog" event package NOTIFY body (application/dialog-info+xml,
// RFC 4235) such as the ones Asterisk's res_pjsip_exten_state sends for a
// hint's BLF state, into the app's own LinphoneEnums::BlfState. Kept
// standalone (not inlined into BlfSubscription) so it can be exercised
// directly against captured NOTIFY bodies.
//
// NOTE: the exact shape Asterisk emits (state values, whether an idle
// extension sends an empty <dialog-info> or a <dialog><state>terminated
// one, early vs proceeding for ringing, etc.) has not been captured and
// verified against a real Asterisk instance for this feature yet -- this
// parser is written defensively against the RFC 4235 baseline and should
// be checked against a real SUBSCRIBE/NOTIFY capture (e.g. via Orbit's
// on-demand SIP capture diagnostic) before relying on it in production.
namespace DialogInfoParser {
LinphoneEnums::BlfState parse(const QByteArray &xml);
}

#endif
