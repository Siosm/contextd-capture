#include <QDesktopServices>
#include <QSettings>
#include <QFileInfo>
#include "editorconfig.h"

EditorConfig::EditorConfig() : _file_watcher(new QFileSystemWatcher())
{
	//Make sure the file exists so that mupuf's horrible file watcher doesn't bug
	QFile settingsFile(QSettings().fileName());
	settingsFile.open(QFile::Append);
	settingsFile.close();

	//Watch for settings updates from other instances
//	_file_watcher->addPath(QSettings().fileName());
//	connect(_file_watcher, SIGNAL(fileChanged(QString)), this, SIGNAL(updated()));
}

EditorConfig::~EditorConfig()
{
}

EditorConfig &EditorConfig::instance()
{
	static EditorConfig conf;

	return conf;
}

void EditorConfig::setLastOpenedPath(QString path)
{
	path=QFileInfo(path).canonicalPath();

	if (path!=QString())
		QSettings().setValue("lastOpenedPath", path);
}

QString EditorConfig::lastOpenedPath()
{
	QString lastPath=QSettings().value("lastOpenedPath").toString();

	if(!QFileInfo(lastPath).exists())
	{
		QString path=QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
		lastPath=path;
		QSettings().setValue("lastOpenedPath", path);
	}

	return lastPath;
}

void EditorConfig::setLastSavedPath(QString path)
{
	path=QFileInfo(path).canonicalPath();

	if (path!=QString())
		QSettings().setValue("lastSavedPath", path);
}

QString EditorConfig::lastSavedPath()
{
	QString lastPath=QSettings().value("lastSavedPath").toString();

	if(!QFileInfo(lastPath).exists())
	{
		QString path=QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
		lastPath=path;
		QSettings().setValue("lastSavedPath", path);
	}

	return lastPath;
}

void EditorConfig::setMaxStackSize(int val)
{
	if(val>10 && val<10000)
		QSettings().setValue("maxStackSize", val);
}

int EditorConfig::maxStackSize()
{
	return QSettings().value("maxStackSize", 100).toInt();
}

void EditorConfig::setAutosaveFrequency(int val)
{
	if(val>=AutosaveFrequencyMin && val<AutosaveFrequencyMax)
		QSettings().setValue("autosaveFrequency", val);
}

int EditorConfig::autosaveFrequency()
{
	return QSettings().value("autosaveFrequency", AutosaveFrequencyDefault).toInt();
}

void EditorConfig::setReopenLastFile(bool reopen)
{
	QSettings().setValue("reopenLastFile", reopen);
}

bool EditorConfig::reopenLastFile()
{
	return QSettings().value("reopenLastFile", ReopenLastFileDefault).toBool();
}

void EditorConfig::setArrowProgramMode(int val)
{
	if(val>=0 && val<InvalidArrowMode)
		QSettings().setValue("arrowProgramMode", val);
}

int EditorConfig::arrowProgramMode()
{
	return QSettings().value("arrowProgramMode", ArrowProgramModeDefault).toInt();
}

void EditorConfig::setHideMode(int val)
{
	if(val>=0 && val<InvalidHideMode)
		QSettings().setValue("hideMode", val);
}

int EditorConfig::hideMode()
{
	return QSettings().value("hideMode", HideModeDefault).toInt();
}

void EditorConfig::setDisplayLoops(bool display)
{
	QSettings().setValue("displayLoops", display);
}

bool EditorConfig::displayLoops()
{
	return QSettings().value("displayLoops", DisplayLoopsDefault).toBool();
}

void EditorConfig::setFastDraw(bool draw)
{
	QSettings().setValue("fastDraw", draw);
}

bool EditorConfig::fastDraw()
{
	return QSettings().value("fastDraw", FastDrawDefault).toBool();
}

void EditorConfig::setArrowColorScheme(int val)
{
	if(val>=0 && val<InvalidColorScheme)
		QSettings().setValue("arrowColorScheme", val);
}

int EditorConfig::arrowColorScheme()
{
	return QSettings().value("arrowColorScheme", ArrowColorSchemeDefault).toInt();
}

void EditorConfig::setDomainColorScheme(int val)
{
	if(val>=0 && val<InvalidColorScheme)
		QSettings().setValue("domainColorScheme", val);
}

int EditorConfig::domainColorScheme()
{
	return QSettings().value("domainColorScheme", DomainColorSchemeDefault).toInt();
}

void EditorConfig::setShowConfRelInDomLabel(bool show)
{
	QSettings().setValue("showConfRelInDomLabel", show);
}

bool EditorConfig::showConfRelInDomLabel()
{
	return QSettings().value("showConfRelInDomLabel", ShowConfRelInDomLabelDefault).toBool();
}

void EditorConfig::setUnreachableDomainWarn(bool warn)
{
	QSettings().setValue("unreachableDomainWarn", warn);
}

bool EditorConfig::unreachableDomainWarn()
{
	return QSettings().value("unreachableDomainWarn", UnreachableDomainWarnDefault).toBool();
}

void EditorConfig::setUnreachableRuleWarn(bool warn)
{
	QSettings().setValue("unreachableRuleWarn", warn);
}

bool EditorConfig::unreachableRuleWarn()
{
	return QSettings().value("unreachableRuleWarn", UnreachableRuleWarnDefault).toBool();
}

void EditorConfig::setBypassedRuleWarn(bool warn)
{
	QSettings().setValue("bypassedRuleWarn", warn);
}

bool EditorConfig::bypassedRuleWarn()
{
	return QSettings().value("bypassedRuleWarn", BypassedRuleWarnDefault).toBool();
}

void EditorConfig::setConfidentialityLossRuleWarn(bool warn)
{
	QSettings().setValue("confidentialityLossRuleWarn", warn);
}

bool EditorConfig::confidentialityLossRuleWarn()
{
	return QSettings().value("confidentialityLossRuleWarn", ConfidentialityLossRuleWarnDefault).toBool();
}

void EditorConfig::setReliabilityLossRuleWarn(bool warn)
{
	QSettings().setValue("reliabilityLossRuleWarn", warn);
}

bool EditorConfig::reliabilityLossRuleWarn()
{
	return QSettings().value("reliabilityLossRuleWarn", ReliabilityLossRuleWarnDefault).toBool();
}

void EditorConfig::setUnknownProgramWarn(bool warn)
{
	QSettings().setValue("unknownProgramWarn", warn);
}

bool EditorConfig::unknownProgramWarn()
{
	return QSettings().value("unknownProgramWarn", UnknownProgramWarnDefault).toBool();
}

void EditorConfig::setUndefinedActionWarn(bool warn)
{
	QSettings().setValue("undefinedActionWarn", warn);
}

bool EditorConfig::undefinedActionWarn()
{
	return QSettings().value("undefinedActionWarn", UndefinedActionWarnDefault).toBool();
}

void EditorConfig::setInvalidMRuleWarn(bool warn)
{
	QSettings().setValue("invalidMRuleWarn", warn);
}

bool EditorConfig::invalidMRuleWarn()
{
	return QSettings().value("invalidMRuleWarn", InvalidMRuleWarnDefault).toBool();
}

void EditorConfig::setDuplicateMRuleWarn(bool warn)
{
	QSettings().setValue("duplicateMRuleWarn", warn);
}

bool EditorConfig::duplicateMRuleWarn()
{
	return QSettings().value("duplicateMRuleWarn", DuplicateMRuleWarnDefault).toBool();
}

void EditorConfig::setOutlineArrowPrograms(bool outline)
{
	QSettings().setValue("outlineArrowPrograms", outline);
}

bool EditorConfig::outlineArrowPrograms()
{
	return QSettings().value("outlineArrowPrograms", OutlineArrowProgramsDefault).toBool();
}

void EditorConfig::onRestoreDefaults()
{
	setAutosaveFrequency(AutosaveFrequencyDefault);
	setReopenLastFile(ReopenLastFileDefault);
	setArrowProgramMode(ArrowProgramModeDefault);
	setHideMode(HideModeDefault);
	setDisplayLoops(DisplayLoopsDefault);
	setFastDraw(FastDrawDefault);
	setArrowColorScheme(ArrowColorSchemeDefault);
	setDomainColorScheme(DomainColorSchemeDefault);
	setShowConfRelInDomLabel(ShowConfRelInDomLabelDefault);
	setUnreachableDomainWarn(UnreachableDomainWarnDefault);
	setUnreachableRuleWarn(UnreachableRuleWarnDefault);
	setBypassedRuleWarn(BypassedRuleWarnDefault);
	setConfidentialityLossRuleWarn(ConfidentialityLossRuleWarnDefault);
	setReliabilityLossRuleWarn((ReliabilityLossRuleWarnDefault));
	setUnknownProgramWarn(UnknownProgramWarnDefault);
	setUndefinedActionWarn(UndefinedActionWarnDefault);
	setInvalidMRuleWarn(InvalidMRuleWarnDefault);
	setDuplicateMRuleWarn(DuplicateMRuleWarnDefault);
	setOutlineArrowPrograms(OutlineArrowProgramsDefault);
}
