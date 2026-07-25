import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic as Control

import Linphone
import UtilsCpp
import "qrc:/qt/qml/Linphone/view/Control/Tool/Helper/utils.js" as Utils

// Function keys (BLF/speed-dial/call-park/call-forward) have no detail
// pane -- a tap places a call directly (FunctionKeyCore::call()) -- so
// unlike Contacts/Meetings this page only ever fills leftPanelContent and
// leaves AbstractMainPage's rightPanel unused, the same choice HelpPage.qml
// already makes for a similarly detail-less page.
AbstractMainPage {
	id: mainItem
	showDefaultItem: false

	leftPanelContent: ColumnLayout {
		id: leftPanel
		Layout.fillWidth: true
		Layout.fillHeight: true
		property real leftMargin: Utils.getSizeWithScreenRatio(45)
		property real rightMargin: Utils.getSizeWithScreenRatio(29)
		spacing: Utils.getSizeWithScreenRatio(5)

		Text {
			Layout.fillWidth: true
			Layout.leftMargin: leftPanel.leftMargin
			Layout.rightMargin: leftPanel.rightMargin
			//: "Touches de fonction"
			text: qsTr("function_keys_page_title")
			color: DefaultStyle.main2_700
			font: Typography.h2
		}

		ColumnLayout {
			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.topMargin: Utils.getSizeWithScreenRatio(24)
			visible: AppCpp.functionKeys && AppCpp.functionKeys.count === 0
			spacing: Utils.getSizeWithScreenRatio(16)
			Item { Layout.fillHeight: true }
			Image {
				Layout.alignment: Qt.AlignHCenter
				source: AppIcons.noItemImage
				Layout.preferredWidth: Utils.getSizeWithScreenRatio(200)
				Layout.preferredHeight: Utils.getSizeWithScreenRatio(175)
				fillMode: Image.PreserveAspectFit
			}
			Text {
				Layout.alignment: Qt.AlignHCenter
				//: "Aucune touche de fonction configurée"
				text: qsTr("function_keys_page_empty")
				color: DefaultStyle.main2_500
			}
			Item { Layout.fillHeight: true }
		}

		Control.ScrollView {
			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.topMargin: Utils.getSizeWithScreenRatio(16)
			visible: AppCpp.functionKeys && AppCpp.functionKeys.count > 0
			ListView {
				id: functionKeysListView
				model: AppCpp.functionKeys
				spacing: Utils.getSizeWithScreenRatio(4)
				delegate: FunctionKeyItem {
					width: functionKeysListView.width
					anchors.leftMargin: leftPanel.leftMargin
					anchors.rightMargin: leftPanel.rightMargin
					functionKey: modelData
				}
			}
		}
	}
}
