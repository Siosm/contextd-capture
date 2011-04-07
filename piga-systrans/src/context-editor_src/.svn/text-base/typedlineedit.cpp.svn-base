#include <QPainter>
#include "typedlineedit.h"

TypedLineEdit::TypedLineEdit(QWidget *parent, QString validation, QStringList blacklist, bool regexp_syntax) :
		QLineEdit(parent),
		_blacklist(blacklist),
		_regexp_syntax(regexp_syntax),
		_value_is_valid(false),
		_warning_icon(":/icons/warning"),
		_model(),
		_completer(&_model, this)
{
	setValidationExpression(validation);

	setCompleter(&_completer);
	_completer.setModelSorting(QCompleter::CaseInsensitivelySortedModel);
	_completer.setCaseSensitivity(Qt::CaseInsensitive);
	_completer.setCompletionMode(QCompleter::UnfilteredPopupCompletion);

	connect(this, SIGNAL(textChanged(QString)), this, SLOT(onValueChanged()));
}

bool TypedLineEdit::valueIsValid() const
{
	bool valid=_validator.exactMatch(text()) && !_blacklist.contains(text());

	if(valid && _regexp_syntax)
		valid=QRegExp(text()).isValid();

	return valid;
}

void TypedLineEdit::setValidationExpression(QString validation)
{
	if(validation==QString())
		validation=".*";

	if(QRegExp(validation).isValid())
	{
		_validator.setPattern(validation);
		checkValueValidity();
	}
	else
		qWarning("TypedLineEdit: tried to use invalid RegExp %s as a validator", qPrintable(validation));
}

QString TypedLineEdit::validationExpression() const
{
	return _validator.pattern();
}

void TypedLineEdit::setBlacklist(QStringList blacklist)
{
	_blacklist=blacklist;
	checkValueValidity();
}

QStringList TypedLineEdit::blacklist() const
{
	return _blacklist;
}

void TypedLineEdit::setIsRegexpSyntax(bool syntax)
{
	_regexp_syntax=syntax;
	checkValueValidity();
}

void TypedLineEdit::setAutocompletionList(QStringList words)
{
	qSort(words);
	_model.setStringList(words);
}

QString TypedLineEdit::type() const
{
	return "LineEdit";
}

void TypedLineEdit::checkValueValidity()
{
	bool valid=valueIsValid();

	if(valid && !_value_is_valid)
	{
		_value_is_valid=true;
		setTextMargins(0, 0, 0, 0);
		setBackgroundRole(QPalette::Base);
	}
	else if(!valid && _value_is_valid)
	{
		_value_is_valid=false;
		setTextMargins(0, 0, 18, 0);
		setBackgroundRole(QPalette::ToolTipBase);
	}
}

void TypedLineEdit::paintEvent(QPaintEvent *event)
{
	if(!_value_is_valid)
	{
		QLineEdit::paintEvent(event);
		QPainter painter(this);
		_warning_icon.paint(&painter, width()-20, (height()-16)/2, 16, 16);
	}
	else
	{
		QLineEdit::paintEvent(event);
	}
}

void TypedLineEdit::onValueChanged()
{
	checkValueValidity();
}
