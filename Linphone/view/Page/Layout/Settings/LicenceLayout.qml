import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic as Control

import Linphone
import UtilsCpp
import 'qrc:/qt/qml/Linphone/view/Style/buttonStyle.js' as ButtonStyle
import 'qrc:/qt/qml/Linphone/view/Control/Tool/Helper/utils.js' as Utils

// GPLv3 s.4 requires a copy of the licence to be handed out along with the program, and a
// hyperlink to gnu.org is not a copy — an offline install must still be able to read it.
// The text is embedded in the binary at build time (see qt6_add_resources in
// Linphone/CMakeLists.txt) and displayed here, next to the modification and trademark
// notices that s.5a asks for. Built on the same header/scroll skeleton as
// AbstractSettingsLayout, but full width and without its save button: this is a document,
// not a settings form.
Rectangle {
	id: mainItem
	width: container.width
	height: container.height
	color: DefaultStyle.grey_0

	property var container
	property string titleText
	// Read through C++ rather than QML's XMLHttpRequest: the latter comes back empty on
	// a qrc: URL, which showed the page with every notice in place but no licence at all.
	property string licenceText: UtilsCpp.getLicenceText()

	Control.Control {
		id: header
		anchors.left: parent.left
		anchors.right: parent.right
		leftPadding: Utils.getSizeWithScreenRatio(45)
		rightPadding: Utils.getSizeWithScreenRatio(45)
		z: 1
		background: Rectangle {
			anchors.fill: parent
			color: DefaultStyle.grey_0
		}
		contentItem: ColumnLayout {
			RowLayout {
				Layout.fillWidth: true
				Layout.topMargin: Utils.getSizeWithScreenRatio(20)
				Layout.bottomMargin: Utils.getSizeWithScreenRatio(10)
				spacing: Utils.getSizeWithScreenRatio(5)
				Button {
					Layout.preferredHeight: Utils.getSizeWithScreenRatio(30)
					Layout.preferredWidth: Utils.getSizeWithScreenRatio(30)
					Layout.rightMargin: Utils.getSizeWithScreenRatio(41)
					icon.source: AppIcons.leftArrow
					focus: true
					visible: mainItem.container.depth > 1
					style: ButtonStyle.noBackground
					onClicked: mainItem.container.pop()
					//: Return
					Accessible.name: qsTr("return_accessible_name")
				}
				Text {
					text: mainItem.titleText
					color: DefaultStyle.main2_600
					font: Typography.h3
				}
				Item {
					Layout.fillWidth: true
				}
			}
			Rectangle {
				Layout.fillWidth: true
				height: Utils.getSizeWithScreenRatio(1)
				color: DefaultStyle.main2_500_main
			}
		}
	}

	Control.ScrollView {
		id: scrollView
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: header.bottom
		anchors.bottom: parent.bottom
		anchors.topMargin: Utils.getSizeWithScreenRatio(16)
		anchors.leftMargin: Utils.getSizeWithScreenRatio(45)
		anchors.rightMargin: Utils.getSizeWithScreenRatio(45)
		contentWidth: availableWidth
		clip: true
		Control.ScrollBar.horizontal: ScrollBar {
			active: false
		}

		ColumnLayout {
			width: scrollView.availableWidth - Utils.getSizeWithScreenRatio(15)
			spacing: Utils.getSizeWithScreenRatio(16)

			Text {
				Layout.fillWidth: true
				text: applicationModificationNotice
				color: DefaultStyle.main2_600
				font: Typography.p1b
				wrapMode: Text.WordWrap
			}
			Text {
				Layout.fillWidth: true
				text: applicationTrademarkNotice
				color: DefaultStyle.main2_600
				font: Typography.p1s
				wrapMode: Text.WordWrap
			}
			// The URLs are their own labels: nothing to translate, and the reader sees where
			// the source of this exact build and the online licence live before clicking.
			Repeater {
				model: [applicationSourceUrl, applicationLicenceUrl]
				Text {
					id: linkText
					required property string modelData
					Layout.fillWidth: true
					text: "<a href='" + modelData + "'>" + modelData + "</a>"
					textFormat: Text.RichText
					wrapMode: Text.WrapAnywhere
					color: DefaultStyle.main2_600
					linkColor: DefaultStyle.main1_500_main
					font: Typography.p1s
					onLinkActivated: (link) => Qt.openUrlExternally(link)
					MouseArea {
						anchors.fill: parent
						acceptedButtons: Qt.NoButton
						cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
					}
				}
			}
			Text {
				Layout.fillWidth: true
				Layout.topMargin: Utils.getSizeWithScreenRatio(16)
				Layout.bottomMargin: Utils.getSizeWithScreenRatio(24)
				text: mainItem.licenceText
				textFormat: Text.PlainText
				color: DefaultStyle.main2_600
				font: Typography.p1s
				wrapMode: Text.WordWrap
			}
		}
	}
}
