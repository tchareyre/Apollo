import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic as Control

import Linphone
import UtilsCpp
import "qrc:/qt/qml/Linphone/view/Style/buttonStyle.js" as ButtonStyle
import "qrc:/qt/qml/Linphone/view/Control/Tool/Helper/utils.js" as Utils

// One row in FunctionKeysPage's list: a colored BLF state dot (blf keys
// only), label/target, and a call button -- every function-key type is a
// plain one-tap call to its target (see FunctionKeyCore::call()).
FocusScope {
	id: mainItem
	property var functionKey
	implicitHeight: Utils.getSizeWithScreenRatio(56)

	Accessible.name: functionKey ? functionKey.label : ""
	Accessible.role: Accessible.Button

	RowLayout {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		anchors.leftMargin: Utils.getSizeWithScreenRatio(16)
		anchors.rightMargin: Utils.getSizeWithScreenRatio(16)
		spacing: Utils.getSizeWithScreenRatio(12)

		Rectangle {
			visible: mainItem.functionKey ? mainItem.functionKey.isBlf : false
			Layout.preferredWidth: Utils.getSizeWithScreenRatio(10)
			Layout.preferredHeight: Utils.getSizeWithScreenRatio(10)
			radius: width / 2
			color: mainItem.functionKey ? UtilsCpp.getBlfStateColor(mainItem.functionKey.blfState) : "transparent"
		}

		ColumnLayout {
			spacing: 0
			Layout.fillWidth: true
			Text {
				Layout.fillWidth: true
				text: mainItem.functionKey ? mainItem.functionKey.label : ""
				color: DefaultStyle.main2_700
				font: Typography.h4
				elide: Text.ElideRight
			}
			Text {
				Layout.fillWidth: true
				text: mainItem.functionKey ? mainItem.functionKey.target : ""
				color: DefaultStyle.main2_500
				font: Typography.p2
				elide: Text.ElideRight
			}
		}

		RoundButton {
			icon.source: AppIcons.phone
			style: ButtonStyle.noBackground
			icon.width: Utils.getSizeWithScreenRatio(24)
			icon.height: Utils.getSizeWithScreenRatio(24)
			Accessible.name: functionKey ? functionKey.label : ""
			onClicked: if (mainItem.functionKey) mainItem.functionKey.call()
		}
	}
}
