#ifndef REGEXP_H
#define REGEXP_H

#include <QPair>
#include <QSharedPointer>
#include <QVector>

#include "byteset.h"

/// Forward declaration
class Derivation;

/// A class representing formal languages theory regular expressions
class RegExp {
public:
	/// A character of subexpression of the expression that can be turned into a standard automata
	enum TokenType {
		NoType,			/**< Not a valid token */
		EpsilonType,	/**< The ε expression */
		NormalType,		/**< A single character */
		AnyType,		/**< Any single character of the alphabet */
		BracketType,	/**< One character among a list */
		ExcludeType,	/**< Any character except those among a list */
		LogicalOrType,	/**< Several sub-expressions, of which one must be matched */
		SubExprType		/**< A sub-expression */
	};

	/// An operator that defines what transitions to add between two tokens a and b
	enum Operator {
		ConcatOp,		/**< Links a.final to b.initial */
		KleeneStarOp,	/**< Links a.final to a.initial and b.initial, and a.initial to b.initial */
		OneOrMoreOp,	/**< Links a.final to a.initial and b.initial */
		OneOrZeroOp		/**< Links a.initial and a.final to b.initial */
	};

	/// Forward declaration
	struct Transition;

	/// Function that fetches the next transition to derivate in a list of transitions
	typedef Transition (*TakeNextTransitionFn) (QList<Transition> &);

	/// Function that extracts a transition's pattern out of a bigger pattern (from which the subpattern is removed)
	typedef QString (*PatternExtractFn) (QString &pattern, const Transition &t);

	/// Function that prepends/appends an expression to another expression
	typedef void (*RegExpConcatFn) (RegExp &a, const RegExp &b);

	/// A token, with a given type, data to store characters/characters lists, and sub_expr to store sub-expressions
	class Token {
		friend class RegExp;
		friend class Transition;
	public:
		TokenType type;
		ByteSet data;
		QList<QSharedPointer<RegExp> > sub_exprs;

	private:
		/// Derivations for the token itself (followed by ε RegExp for atomic tokens - may be undeterministic)
		QVector<Derivation> derivations(TakeNextTransitionFn, PatternExtractFn, RegExpConcatFn) const;
	};

	/// A token from a derivation, either AnyType, BracketType or ExcludeType
	struct DerivationToken {
		TokenType type;
		ByteSet data;

		DerivationToken();
		DerivationToken(const Token &token);
		DerivationToken(const TokenType &type, const ByteSet &data);
	};

	/// A transition, made of a Token and the next operator to apply
	struct Transition {
		Token token;
		Operator op;

		/// Assign the transition the ε Token
		inline void setEpsilon();

		/// Whether the transition has the ε Token
		inline bool isEpsilon() const;

		/// Whether the transition is nullable (depending on its token's nullability and on its operator)
		bool isNullable() const;
	};

	/*!
	 * \brief Constructs a RegExp object
	 */
	RegExp();


	/*!
	 * \brief Constructs the ε RegExp object
	 * \param epsilon A dummy parameter for differentiating with the normal constructor
	 */
	RegExp(int epsilon);

	/*!
	 * \brief Constructs a RegExp object from a pattern
	 * \param pattern The pattern to use for initialisation (only Qt::CaseSensitive;QRegExp::RegExp supported for now)
	 */
	RegExp(const QString &pattern);

	/*!
	 * \brief Copy constructs a RegExp object
	 * \param rx The RegExp to copy
	 */
	RegExp(const RegExp &rx);

	/*!
	 * \brief Destructs a RegExp object
	 */
	~RegExp();

	/*!
	 * \brief Changes the pattern of the expression and recalculates its transitions
	 * \param pattern The new pattern
	 */
	inline void setPattern(const QString &pattern);

	/// Turns the expression into the ε expression
	inline void setEpsilon();

	/// Turns the expression into the ø expression
	inline void setNull();

	/// Whether the expression is ε or ø
	inline bool isEmpty() const;

	/// Whether the expression is ε
	inline bool isEpsilon() const;

	/// Whether the expression is ø
	inline bool isNull() const;

	/// Returns the pattern of the object
	inline QString pattern() const;

	/// Returns the transitions matching the current pattern of the object
	inline QList<Transition> transitions() const;

	// TODO doc/code
	QVector<Derivation> leftDerivations() const;
	QVector<Derivation> rightDerivations() const;

	/// Whether the regular expression can recognize 'ε'
	inline bool isNullable() const;

	/// Prepend a regular expression to this expression
	RegExp& prepend(const RegExp &b);

	/// Append a regular expression to this expression
	RegExp& append(const RegExp &b);

	/// Operator < that sorts the RegExp patterns lexicographically (for usage as QMap/QSet keys)
	bool operator <(const RegExp &other) const;

	/// Append another regular expression to the object - will abort if one of the expressions is a LogicalOr
	/// If the expressions make use of '^' and '$', the resulting expression will not be valid (for performance reasons)
	RegExp& operator +=(const RegExp &b);

	/// Transform this expression into a LogicalOr of this and another expression passed as a parameter
	/// If the expressions make use of '^' and '$', the resulting expression will not be valid (for performance reasons)
	RegExp& operator |=(const RegExp &b);

	/// Assign another regular expression to the object
	RegExp& operator =(RegExp b);

	/// Non-exhaustive list of special characters
	const static char AnyCharacter		='.';
	const static char BracketBegin		='[';
	const static char BracketEnd		=']';
	const static char EscapeSymbol		='\\';
	const static char Exclude			='^';
	const static char ExpressionBegin	='^';
	const static char ExpressionEnd		='$';
	const static char LogicalOr			='|';
	const static char KleeneStar		='*';
	const static char OneOrMore			='+';
	const static char OneOrZero			='?';
	const static char SubExprBegin		='(';
	const static char SubExprEnd		=')';
	const static char Range				='-';

private:
	/// The pattern matched by the regular expression
	QString _pattern;

	/// A list for internally storing the transitions matching the pattern
	QList<Transition> _transitions;

	/*!
	 * \brief Constructs a RegExp object with an already calculated list of transitions
	 * \param pattern The pattern that matches the list of transitions (only Qt::CaseSensitive;QRegExp::RegExp supported for now)
	 * \param transitions The already calculated transitions for pattern
	 */
	RegExp(const QString &pattern, const QList<Transition> &transitions);

	/// Returns the list of transitions for a given pattern / sub-pattern
	QList<Transition> parsePattern(const QString &pattern, bool top_level=false) const;

	/// Internal function for calculating the nullability of the transitions
	bool isNullable(int index) const;

	/// Internal function for calculating the nullability of one transition
	static bool isNullable(const Transition &t);

	/// Calculates all the possible derivations of the expression, in an order determined by the passed functions
	QVector<Derivation> derivations(TakeNextTransitionFn, PatternExtractFn, RegExpConcatFn) const;
};

/// Creates a logical OR out of two expressions
/// If the expressions make use of '^' and '$', the resulting expression will not be valid (for performance reasons)
RegExp operator|(const RegExp &a, const RegExp &b);

/// Compares two RegExp objects for use as QMap/QSet keys
bool operator==(const RegExp &a, const RegExp &b);

/// A token and the sub-expression to recognize after the derivation from this token
struct Derivation {
	RegExp::DerivationToken token;
	RegExp expr;

	Derivation();
	Derivation(const RegExp::DerivationToken &token, const RegExp &expr);

	bool isEmpty() const;
};

/// Function qHash used for using RegExp as QMap/QSet keys
inline uint qHash(const RegExp &key)
{
	return qHash(key.pattern());
}

#endif // REGEXP_H
