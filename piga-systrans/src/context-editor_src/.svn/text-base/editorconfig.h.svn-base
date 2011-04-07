#ifndef EDITORCONFIG_H
#define EDITORCONFIG_H

#include <QString>
#include <QStringList>
#include <QFileSystemWatcher>

/// A class for the application settings of Context Editor
class EditorConfig : public QObject
{
Q_OBJECT

public:
	/// Fetch the instance of the configuration
	static EditorConfig &instance();

	/// Last path used for opening files
	static void setLastOpenedPath(QString path);
	static QString lastOpenedPath();

	/// Last path used for saving files
	static void setLastSavedPath(QString path);
	static QString lastSavedPath();

	/// Maximum number of configuration instances in the undo/redo stack
	static void setMaxStackSize(int val);
	static int maxStackSize();

	/// Minutes before the transitions file is automatically saved (0 to disable)
	static void setAutosaveFrequency(int val);
	static int autosaveFrequency();

	/// Whether the last used file should be reopened when launching the app
	static void setReopenLastFile(bool reopen);
	static bool reopenLastFile();

	/// How to display the program icons attached to a GraphicsArrow
	static void setArrowProgramMode(int val);
	static int arrowProgramMode();

	/// What to do when a domain/program/rule is hidden
	static void setHideMode(int val);
	static int hideMode();

	/// Whether to display loops and rules from and to a same domain
	static void setDisplayLoops(bool display);
	static bool displayLoops();

	/// If true, give priority to drawing speed over rendering quality
	static void setFastDraw(bool fast);
	static bool fastDraw();

	/// What color scheme to use for arrows in the view
	static void setArrowColorScheme(int val);
	static int arrowColorScheme();

	/// What color scheme to use for domains in the view
	static void setDomainColorScheme(int val);
	static int domainColorScheme();

	/// Whether to display confidentiality / reliability in domain labels
	static void setShowConfRelInDomLabel(bool show);
	static bool showConfRelInDomLabel();

	/// Whether to warn when unreachable domains are found
	static void setUnreachableDomainWarn(bool warn);
	static bool unreachableDomainWarn();

	/// Whether to warn when unreachable rules are found
	static void setUnreachableRuleWarn(bool warn);
	static bool unreachableRuleWarn();

	/// Whether to warn when a rule is bypassed
	static void setBypassedRuleWarn(bool warn);
	static bool bypassedRuleWarn();

	/// Whether to warn when confidentiality-violating rules are found
	static void setConfidentialityLossRuleWarn(bool warn);
	static bool confidentialityLossRuleWarn();

	/// Whether to warn when reliability-violating rules are found
	static void setReliabilityLossRuleWarn(bool warn);
	static bool reliabilityLossRuleWarn();

	/// Whether to warn when a rule's program is not known
	static void setUnknownProgramWarn(bool warn);
	static bool unknownProgramWarn();

	/// Whether to warn when a rule contains mrules with no actions
	static void setUndefinedActionWarn(bool warn);
	static bool undefinedActionWarn();

	/// Whether to warn when a rule contains invalid mrules
	static void setInvalidMRuleWarn(bool warn);
	static bool invalidMRuleWarn();

	/// Whether to warn when a rule's MRule is a subset of another rule's MRule
	static void setDuplicateMRuleWarn(bool warn);
	static bool duplicateMRuleWarn();

	/// Whether to draw a border for arrow programs containing rules with warnings
	static void setOutlineArrowPrograms(bool outline);
	static bool outlineArrowPrograms();

	static const int AutosaveFrequencyMin=0;
	static const int AutosaveFrequencyDefault=2;
	static const int AutosaveFrequencyMax=14400;

	static const bool DisplayInvalidRulesDefault=true;
	static const bool ReopenLastFileDefault=false;
	static const bool DisplayLoopsDefault=false;
	static const bool FastDrawDefault=false;
	static const bool ShowConfRelInDomLabelDefault=true;
	static const bool BypassedRuleWarnDefault=true;
	static const bool UnreachableDomainWarnDefault=true;
	static const bool UnreachableRuleWarnDefault=true;
	static const bool ConfidentialityLossRuleWarnDefault=true;
	static const bool ReliabilityLossRuleWarnDefault=true;
	static const bool UnknownProgramWarnDefault=true;
	static const bool UndefinedActionWarnDefault=true;
	static const bool InvalidMRuleWarnDefault=true;
	static const bool DuplicateMRuleWarnDefault=true;
	static const bool OutlineArrowProgramsDefault=true;

	enum {
		HorizontalIcons=0,
		Popup,
		InvalidArrowMode
	} ArrowProgramMode;
	static const int ArrowProgramModeDefault=HorizontalIcons;

	enum {
		LowerOpacity=0,
		LowerOpacityAndForbidSelection,
		RemoveFromView,
		InvalidHideMode
	} HideMode;
	static const int HideModeDefault=LowerOpacity;

	enum {
		NoColorScheme=0,
		ConfidentialityBased,
		ReliabilityBased,
		WarningColor,
		InvalidColorScheme
	} ColorScheme;
	static const int ArrowColorSchemeDefault=WarningColor;
	static const int DomainColorSchemeDefault=WarningColor;

private:
	EditorConfig();
	~EditorConfig();

	QFileSystemWatcher* _file_watcher;

signals:
	void updated();

public slots:
	/// Restore the default value of each setting
	void onRestoreDefaults();
};

#endif // EDITORCONFIG_H
