#ifndef TYPEDLINEEDIT_H
#define TYPEDLINEEDIT_H

#include <QCompleter>
#include <QIcon>
#include <QLineEdit>
#include <QRegExp>
#include <QStringListModel>

/// A line edit that checks the data it is given matches a regular expression and has a regexp syntax, with an optional blacklist
class TypedLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	/*!
	 * \brief Construct a Typed LineEdit with data integrity checking
	 * \param parent The parent widget of this item
	 * \param validation A regexp string that the data of this item must match
	 * \param blacklist A list of forbidden values for the data of this item
	 * \param regexp_syntax Whether the data must be a syntaxically valid regexp
	 */
	explicit TypedLineEdit(QWidget *parent = 0, QString validation = ".*", QStringList blacklist=QStringList(), bool regexp_syntax=false);

	/// Tell whether the current value is valid
	bool valueIsValid() const;

	/// Set a new expression to use for validation
	void setValidationExpression(QString validation);
	QString validationExpression() const;

	/// Set a new blacklist to use for validation
	void setBlacklist(QStringList blacklist);
	QStringList blacklist() const;

	/// Tell whether the value must be a syntaxically valid regexp
	void setIsRegexpSyntax(bool syntax);

	/// Set a list of words to propose for autocompletion
	void setAutocompletionList(QStringList words);

	/// String representation of the widget
	QString type() const;

private:
	/// An expression that must be matched for the data to be valid
	QRegExp _validator;
	/// A list of forbidden values
	QStringList _blacklist;
	/// Whether the value must be a syntaxically valid regexp
	bool _regexp_syntax;

	/// Check the validity of the value and cache the result for the paint event
	/// Also change the widget so that the next paint event will notify the validity of the value
	void checkValueValidity();
	bool _value_is_valid;

	/// A warning icon for when the value is not valid
	QIcon _warning_icon;

	/// A QCompleter for suggesting words
	QStringListModel _model;
	QCompleter _completer;

	/// Redeclaration of the paint event, used to inject a warning icon if the value is not valid
	void paintEvent(QPaintEvent *event);

private slots:
	void onValueChanged();
};

#endif // TYPEDLINEEDIT_H
